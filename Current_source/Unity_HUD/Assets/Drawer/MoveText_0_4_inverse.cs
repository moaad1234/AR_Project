using UnityEngine;
using System.Collections.Generic;

public class MoveText_0_4_inverse : MonoBehaviour {

	private int MoveSpeed_local;

	void Update(){
		MoveSpeed_local = RotateCube_0_2.Instance.MoveSpeed;
		float Spd = MoveSpeed_local * Time.deltaTime; //conversion of move speed from unit/sec to a coordinate
		
		if (RotateCube_0_2.Instance.CalibComplete == false) {
			//movement in z axis (forwards and backwards)
			if (Input.GetKey (KeyCode.W)) {
				if (Input.GetKey (KeyCode.LeftShift))
					transform.position += new Vector3 (0, 0, Spd * 2);
				else
					transform.position += new Vector3 (0, 0, Spd);
			}
			if (Input.GetKey (KeyCode.S)) {
				if (Input.GetKey (KeyCode.LeftShift))
					transform.position -= new Vector3 (0, 0, Spd * 2);
				else
					transform.position -= new Vector3 (0, 0, Spd);
			}

			//movement in x axis (left and right)
			if (Input.GetKey (KeyCode.A)) {
				if (Input.GetKey (KeyCode.LeftShift))
					transform.position -= new Vector3 (Spd * 2, 0, 0);
				else
					transform.position -= new Vector3 (Spd, 0, 0);
			}
			if (Input.GetKey (KeyCode.D)) {
				if (Input.GetKey (KeyCode.LeftShift))
					transform.position += new Vector3 (Spd * 2, 0, 0);
				else
					transform.position += new Vector3 (Spd, 0, 0);
			}

			//movement in y axis (up and down)
			if (Input.GetKey (KeyCode.Q)) {
				if (Input.GetKey (KeyCode.LeftShift))
					transform.position += new Vector3 (0, Spd * 2, 0);
				else
					transform.position += new Vector3 (0, Spd, 0);
			}
			if (Input.GetKey (KeyCode.E)) {
				if (Input.GetKey (KeyCode.LeftShift))
					transform.position -= new Vector3 (0, Spd * 2, 0);
				else
					transform.position -= new Vector3 (0, Spd, 0);
			}
		} 
		else {
			GetComponent<MeshRenderer>().enabled = true;
			GetComponent<rwyRemaining> ().enabled = true;
		}
	}
}