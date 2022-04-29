using UnityEngine;
using System.Collections.Generic;

public class RotateCube_0_1 : MonoBehaviour {
	
	public int MoveSpeed;
	private Vector3 RotateAmount;

	void Update(){
		
		float Spd = MoveSpeed * Time.deltaTime; //conversion of move speed from unit/sec to a coordinate
		RotateAmount = new Vector3(Spd, Spd, Spd);
		transform.Rotate (RotateAmount);

		//movement in z axis (forwards and backwards)
		if (Input.GetKey (KeyCode.W))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.Translate (0, 0, Spd);
			else
				transform.Translate (0, 0, Spd/2);
		if (Input.GetKey (KeyCode.S))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.Translate (0, 0, Spd / 2 * -1);
			else
				transform.Translate (0, 0, Spd * -1);

		//movement in x axis (left and right)
		if (Input.GetKey (KeyCode.A))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.Translate (Spd/2, 0, 0);
			else
				transform.Translate (Spd, 0, 0);
		if (Input.GetKey (KeyCode.D))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.Translate (Spd / 2 * -1, 0, 0);
			else
				transform.Translate (Spd * -1, 0, 0);
		
		//movement in y axis (up and down)
		if (Input.GetKey (KeyCode.Q))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.Translate(0,Spd/2,0);
			else
				transform.Translate (0, Spd, 0);
		if (Input.GetKey (KeyCode.E))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.Translate (0, Spd / 2 * -1, 0);
			else
				transform.Translate (0, Spd * -1, 0); 

	}
}