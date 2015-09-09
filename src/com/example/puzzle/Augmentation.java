/*
 * Interface package between Android client and c++ functions
 */
package com.example.puzzle;

public class Augmentation {
	//function do the OR and AR using image moments and knn for the recognition of the parts
	public native static void classify(long matAddrRgba);
	
	public native static int training();
	
	public native static void next();
	
	
	static{
		System.loadLibrary("AR");
	}
	
}
