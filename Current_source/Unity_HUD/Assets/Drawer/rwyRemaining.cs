using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class rwyRemaining : MonoBehaviour {

	[DllImport("rwyRemaining_toUnity")]
	private static extern int rwyRemaining_toUnity ();
	int distance;
	float homeX;
	float homeY;
	float homeZ;

	// Use this for initialization
	void Start () {
		StartCoroutine (CalibCheck());
		homeX = transform.position.x;
		homeY = transform.position.y;
		homeZ = transform.position.z;
	}

	IEnumerator CalibCheck(){
		yield return new WaitUntil (IsCalibComplete);
	}

	// Update is called once per frame
	void Update () {
		distance = rwyRemaining_toUnity ();
		transform.position = new Vector3(homeX,homeY,homeZ + distance);
	}
		
	public bool IsCalibComplete(){
		if (RotateCube_0_2.Instance.CalibComplete == true) {
			return true;
		} 
		else {
			return false;	
		}
	}
}	
	
