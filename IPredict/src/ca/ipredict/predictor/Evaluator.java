package ca.ipredict.predictor;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.io.IOException;
import java.io.*;
import java.util.*;

import ca.ipredict.database.DatabaseHelper;
import ca.ipredict.database.DatabaseHelper.Format;
import ca.ipredict.database.Item;
import ca.ipredict.database.Sequence;
import ca.ipredict.database.SequenceStatsGenerator;
import ca.ipredict.helpers.MemoryLogger;
import ca.ipredict.helpers.StatsLogger;
import ca.ipredict.predictor.profile.Profile;
import ca.ipredict.predictor.profile.ProfileManager;

/**
 * Evaluation framework
 */
public class Evaluator {

	private static final boolean ALLOW_SPLIT_LENGTH_TRAINING_DATA_EXPORT = true;
	private static boolean print = true;

	private List<Predictor> predictors; //list of predictors
	
	//Sampling type
	public final static int HOLDOUT = 0;
	public final static int KFOLD = 1;
	public final static int RANDOMSAMPLING = 2; 
	
	//statistics
	private long startTime;
	private long endTime;
	
	//Database
	private DatabaseHelper database;
	
	//public Stats stats;
	public StatsLogger stats;
	public List<StatsLogger> experiments;
	
	public List<String> datasets;  
	public List<Integer> datasetsMaxCount;  
	
	
	public Evaluator(String pathToDatasets) {
		predictors = new ArrayList<Predictor>();
		datasets = new ArrayList<String>();
		datasetsMaxCount = new ArrayList<Integer>();
		database = new DatabaseHelper(pathToDatasets);
	}
	
	/**
	 * Adds a Predictor to the list of predictors
	 * @param predictor
	 */
	public void addPredictor(Predictor predictor) {
		predictors.add(predictor);
	}
	
	/**
	 * Adds a dataset to the experiment
	 * @param format Format of the Dataset
	 * @param maxCount Maximum number of sequence to read in the dataset
	 */
	public void addDataset(String format, int maxCount) {
		datasets.add(format);
		datasetsMaxCount.add(maxCount);
	}

	/**
	 * Start the controller using the prefered SamplingRate on the list of predictor
	 * @param samplingType one of: HOLDOUT, RANDOMSAMPLING, KFOLD
	 * @param param The parameter associated with the sampling type
	 * @param showDatasetStats show statistics about the dataset
	 * @return 
	 */
	public StatsLogger Start(int samplingType, float param, boolean showResults, boolean showDatasetStats, boolean showExecutionStats) throws IOException {
	
		//Setting statsLogger
		List<String> statsColumns = new ArrayList<String>();
		statsColumns.add("Success");
		statsColumns.add("Failure");
		statsColumns.add("No Match");
		statsColumns.add("Too Small");
		statsColumns.add("Overall");
		statsColumns.add("Size (MB)");
		statsColumns.add("Train Time");
		statsColumns.add("Test Time");
		
		//Extracting the name of each predictor
		List<String> predictorNames = new ArrayList<String>();
		for(Predictor predictor : predictors) {
			predictorNames.add(predictor.getTAG());
		}

		
		for(int i = 0; i < datasets.size(); i++) {
			
			int maxCount = datasetsMaxCount.get(i);
			String format = datasets.get(i);
			
			//Loading the parameter profile
			ProfileManager.loadProfileByName(format.toString());
			
			//Loading the dataset
			database.loadDataset(format, maxCount);

			database.getDatabase().normaliseSequenceData();
			System.out.println("Seperator character / Sigma size: " + database.getDatabase().sigma);
			
			if(showDatasetStats) {
				System.out.println();
				SequenceStatsGenerator.prinStats(database.getDatabase(), format);
			}
			
			//Creating the statsLogger
			stats = new StatsLogger(statsColumns, predictorNames, false);
			
			//Saving current time for across time analysis
			startTime = System.currentTimeMillis();
			
			//For each predictor, do the sampling and do the training/testing
			for(int id = 0 ; id < predictors.size(); id++) {
				
				//Picking the sampling strategy
				switch(samplingType) {
					case HOLDOUT:
						Holdout(param, id);
						break;
				
					case KFOLD:
						KFold((int)param, id, format);
						break;
						
					case RANDOMSAMPLING:
						RandomSubSampling(param, id);
						break;
					
					default: 
						System.out.println("Unknown sampling type."); 
				}
			}
			//Saving end time
			endTime = System.currentTimeMillis();
			
			finalizeStats(showExecutionStats);
			
			if(showResults == true) {
				System.out.println(stats.toString());
			}
		}
		
		return stats;
	}

	/**
	 * Holdout method
	 * Data are randomly partitioned into two sets (a training set and a test set) using a ratio.
	 * The classifier is trained using the training set and evaluated using the test set.
	 * @param ratio to divide the training and test sets
	 */
	public void Holdout(double ratio, int classifierId) {
		
		List<Sequence> trainingSequences = getDatabaseCopy();
		List<Sequence> testSequences = splitList(trainingSequences, ratio);
		
		//DEBUG
		//System.out.println("Dataset size: "+ (trainingSequences.size() + testSequences.size()));
		//System.out.println("Training: " + trainingSequences.size() + " and Test set: "+ testSequences.size());
		
		PrepareClassifier(trainingSequences, classifierId); //training (preparing) classifier
		
		StartClassifier(testSequences, classifierId); //classification of the test sequence
	}
	
	/**
	 * Random subsampling
	 * Holdout method repeated 10 times
	 * @param ratio to use for the holdout method
	 */
	public void RandomSubSampling(double ratio, int classifierId) {
		
		int k = 10;
		for(int i = 0 ; i < k; i++) {
			Holdout(ratio, classifierId);
			
			//Logging memory usage
			MemoryLogger.addUpdate();
		}
		
	}
	
	/**
	 * k-fold cross-validation
	 * Data are partitioned in k exclusive subsets (folds) of same size.
	 * Training and testing is done k times. For each time; a fold is used for testing 
	 * and the k-1 other folds for training
	 */
	public void KFold(int k, int classifierId, String datasetName) {
		
		//k has to be at least 2
		if(k < 2) {
			throw new RuntimeException("K needs to be 2 or more");
		}

		List<Sequence> dataSet = getDatabaseCopy();
		
		//calculating absolute ratio
		double relativeRatio = 1/(double)k;
		int absoluteRatio = (int) (dataSet.size() * relativeRatio);
		
		//For each fold, it does training and testing
		for(int i = 0 ; i < k ; i++) {

			//Partitioning database 
			//
			int posStart = i * absoluteRatio; //start position of testing set
			int posEnd = posStart + absoluteRatio; //end position of testing set
			if(i == (k-1)) { //if last fold we adjust the size to include all the left-over sequences
				posEnd = dataSet.size(); //special case
			}
			
			//declaring the sets
			List<Sequence> trainingSequences = new LinkedList<Sequence>();
			List<Sequence> testSequences = new LinkedList<Sequence>();
			
			//actual partitioning
			for(int j = 0 ; j < dataSet.size(); j++) {
				
				Sequence toAdd = dataSet.get(j);
				
				//is in testing set
				if(j >= posStart && j < posEnd) {
					testSequences.add(toAdd);
				}
				else {
					trainingSequences.add(toAdd);
				}
			}
			//
			//End of Partitioning
		
			PrepareClassifier(trainingSequences, classifierId); //training (preparing) classifier	
			StartClassifier(testSequences, classifierId); //classification of the test sequence

			writeFoldsAndAnswers(trainingSequences, testSequences, i, datasetName);
			
			//Logging memory usage
			MemoryLogger.addUpdate();
		}
		
	}

	private Sequence keepLastItems(Sequence sequence, int length) { 

		if(sequence.size() <= length){ 
			return sequence;
		}
		
		//slicing the seqence
		Sequence result = new Sequence(sequence.getId(), sequence.getItems().subList(sequence.size() - length, sequence.size()));
		return result;
	}

	public void writeFoldsAndAnswers(List<Sequence> trainingSequences, List<Sequence> testingSequences, int fold, String datasetName){
		
		// Assume default encoding.

		FileWriter fileWriter = null, fileWriter2 = null;
		BufferedWriter bufferedWriter = null, bufferedWriter2 = null;

		String fileName = "outputs/" + datasetName + ".fold." + fold + ".training.mapped.txt";
		try{
	        fileWriter =
	            new FileWriter(fileName);
	        // Always wrap FileWriter in BufferedWriter.
	       	bufferedWriter =
	            new BufferedWriter(fileWriter);
	    }catch(IOException ex){

	    }
		

	    try { bufferedWriter.write(database.getDatabase().sigma + " "); bufferedWriter.newLine();}catch(IOException ex) { System.out.println(
	                "Error writing to file '"
	                + fileName + "'");}

		for(Sequence target : trainingSequences) {
			
			if(Profile.paramInt("splitMethod") > 0 && ALLOW_SPLIT_LENGTH_TRAINING_DATA_EXPORT) {
				if (print) {System.out.println("Active splitLength during export of training data"); print = false;}
				target = keepLastItems(target, Profile.paramInt("splitLength"));
			}else{
				if (print) {System.out.println("No active splitLength during export of training data"); print = false;}
			}

			//write in a file the training sequence e.g Fifa.fold.1.training.txt
	        try {

	            bufferedWriter.write(target.toString() + database.getDatabase().sigma + " ");

	        }
	        catch(IOException ex) {
	            System.out.println(
	                "Error writing to file '"
	                + fileName + "'");
	            // Or we could just do this:
	            // ex.printStackTrace();
	        }

		}

		try {bufferedWriter.close();} catch (IOException ex){}


		fileName = "outputs/" + datasetName + ".fold." + fold + ".consequent.mapped.txt";


        String fileName2 = "outputs/" + datasetName + ".fold." + fold + ".queries.mapped.txt";

        try{

        	fileWriter =
	            new FileWriter(fileName);
	        bufferedWriter =
	        	new BufferedWriter(fileWriter);

	        fileWriter2 =
	            new FileWriter(fileName2);
	        bufferedWriter2 =
	            new BufferedWriter(fileWriter2);
	    }catch(IOException ex){}


		for(Sequence target : testingSequences) {
			
			//if sequence is long enough
			if(target.size() > (Profile.paramInt("consequentSize"))) {
				
				Sequence consequent = target.getLastItems(Profile.paramInt("consequentSize"),0); //the lasts actual items in target
				Sequence finalTarget = target.getLastItems(Profile.paramInt("windowSize"),Profile.paramInt("consequentSize")); //the actual query
				


		        try {
		           
		            bufferedWriter.write(consequent.toString());
		            bufferedWriter.newLine();
		            
		        }
		        catch(IOException ex) {
		            System.out.println(
		                "Error writing to file '"
		                + fileName + "'");
		       
		        }



		        try {
		           
		            bufferedWriter2.write(finalTarget.toString());
		            bufferedWriter2.newLine();
		            
		        }
		        catch(IOException ex) {
		            System.out.println(
		                "Error writing to file '"
		                + fileName + "'");
		        }
			}
		}

		try {bufferedWriter.close();} catch (IOException ex){}
		try {bufferedWriter2.close();} catch (IOException ex){}
	}
	
	/**
	 * Display the stats for the experiment
	 * @param showExecutionStats
	 */
	public void finalizeStats(boolean showExecutionStats) {
		
		//For each predictor, updates the stats
		for(Predictor predictor : predictors) {
			
			int success = (int)(stats.get("Success", predictor.getTAG()));
			int failure = (int)(stats.get("Failure", predictor.getTAG()));
			int noMatch = (int)(stats.get("No Match", predictor.getTAG()));
			int tooSmall =(int)(stats.get("Too Small", predictor.getTAG()));
			
			
			long matchingSize = success + failure; //For relative success (success / (success + failure))
			long testingSize = matchingSize + noMatch + tooSmall; //For global success (success / All_the_testing)
			
			stats.divide("Success", predictor.getTAG(), matchingSize);
			stats.divide("Failure", predictor.getTAG(), matchingSize);
			stats.divide("No Match", predictor.getTAG(), testingSize);
			stats.divide("Too Small", predictor.getTAG(), testingSize);
			
			stats.divide("Train Time", predictor.getTAG(), 100);
			stats.divide("Test Time", predictor.getTAG(), 100);
			
			//Adding overall success
			stats.set("Overall", predictor.getTAG(), success);
			stats.divide("Overall", predictor.getTAG(), testingSize);
			
			//Size of the predictor
			stats.set("Size (MB)", predictor.getTAG(), predictor.memoryUsage());
			stats.divide("Size (MB)", predictor.getTAG(), (100 * 1000 * 1000));
			
			
		}
		
		if(showExecutionStats) {
	        //memory usage
	  		MemoryLogger.addUpdate();
	        MemoryLogger.displayUsage();
	        
	        //Displaying the execution time
	        System.out.println("Execution time: "+ (endTime - startTime) / 1000 + " seconds");
		}
	}
	
	/**
	 * Tell whether the predicted sequence match the consequent sequence
	 */
	public static Boolean isGoodPrediction(Sequence consequent, Sequence predicted) {
		
		Boolean hasError = false;
		
		for(Item it : predicted.getItems()) {
			
			Boolean isFound = false;
			for(Item re : consequent.getItems()) {
				if( re.val.equals(it.val) )
					isFound = true;
			}
			if(isFound == false)
				hasError = true;
			
		}
		
		
		return (hasError == false);
	}
	

	
	private void PrepareClassifier(List<Sequence> trainingSequences, int classifierId) {
		long start = System.currentTimeMillis(); //Training starting time
		
		predictors.get(classifierId).Train(trainingSequences); //actual training
		
		long end = System.currentTimeMillis(); //Training ending time
		double duration = (double)(end - start) / 1000;
		stats.set("Train Time", predictors.get(classifierId).getTAG(), duration);
	}
	
	private void StartClassifier(List<Sequence> testSequences, int classifierId) {	
		
		long start = System.currentTimeMillis(); //Testing starting time

		int success = 0, failure = 0, noMatch = 0, tooSmall = 0;
		
		//for each sequence; it classifies it and evaluates it
		for(Sequence target : testSequences) {
			
			//if sequence is long enough
			if(target.size() > (Profile.paramInt("consequentSize"))) {
				
				Sequence consequent = target.getLastItems(Profile.paramInt("consequentSize"),0); //the lasts actual items in target
				Sequence finalTarget = target.getLastItems(Profile.paramInt("windowSize"),Profile.paramInt("consequentSize"));
				
				Sequence predicted = predictors.get(classifierId).Predict(finalTarget);
				
				//if no sequence is returned, it means that they is no match for this sequence
				if(predicted.size() == 0) {
					stats.inc("No Match", predictors.get(classifierId).getTAG());
					noMatch++;
				}
				//evaluates the prediction
				else if(isGoodPrediction(consequent, predicted)) {
					stats.inc("Success", predictors.get(classifierId).getTAG());
					success++;
				}
				else {
					stats.inc("Failure", predictors.get(classifierId).getTAG());
					failure++;
				}
				
			}
			//sequence is too small
			else {
				stats.inc("Too Small", predictors.get(classifierId).getTAG());
				tooSmall++;
			}
		}

		long end = System.currentTimeMillis(); //Training ending time
		//System.out.println(predictors.get(classifierId).getTAG() + " memory (MB): " + predictors.get(classifierId).memoryUsage() / (1000 * 1000));
		//System.out.println(predictors.get(classifierId).getTAG() + " test time (ms): " + (double)(end - start));
		double duration = (double)(end - start) / 1000;
		System.out.println(predictors.get(classifierId).getTAG() + ":" + duration);
		stats.set("Test Time", predictors.get(classifierId).getTAG(), duration);

	}

	private List<Sequence> splitList(List<Sequence> toSplit, double absoluteRatio){
		
		int relativeRatio = (int) (toSplit.size() * absoluteRatio); //absolute ratio: [0.0-1.0]
		
		List<Sequence> sub=toSplit.subList(relativeRatio , toSplit.size());
		List<Sequence> two= new ArrayList<Sequence>(sub);
		sub.clear();
		
		return two;
	}
	
	private List<Sequence> getDatabaseCopy() {
		return new ArrayList<Sequence>(database.getDatabase().getSequences().subList(0, database.getDatabase().size()));
	}
	
}
