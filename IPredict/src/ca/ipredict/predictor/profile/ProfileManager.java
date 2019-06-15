package ca.ipredict.predictor.profile;

import ca.ipredict.predictor.profile.BMSProfile;
import ca.ipredict.predictor.profile.SIGNProfile;
import ca.ipredict.predictor.profile.MSNBCProfile;
import ca.ipredict.predictor.profile.BIBLE_WORDProfile;
import ca.ipredict.predictor.profile.BIBLE_CHARProfile;
import ca.ipredict.predictor.profile.KOSARAKProfile;
import ca.ipredict.predictor.profile.FIFAProfile;
import ca.ipredict.predictor.profile.SPICEProfile;

/**
 * Apply a parameter profile by name
 */
public class ProfileManager {	
	public static void loadProfileByName(String name) {
		Profile profile = null;
		try {
			Class<?> classI;
			if (name.contains("SPICE") ) classI = Class.forName("ca.ipredict.predictor.profile."+ "SPICE" + "Profile");
			else classI = Class.forName("ca.ipredict.predictor.profile."+ name + "Profile");
			profile = (Profile) classI.newInstance();
		} catch (Exception e) {
			System.out.println("------------------ERROR-----DEFAULT PROFILE LOADED------------------");
			profile = new DefaultProfile();
		}
		
		profile.Apply();
	}
}
