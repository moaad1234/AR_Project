using UnityEngine;
using System.Collections.Generic;
using UnityEngine.UI;

public class RotateCube_0_2 : MonoBehaviour {

	public static RotateCube_0_2 Instance { get; private set;}
	public Text CalibText;

	public bool CalibComplete = false;
	public int MoveSpeed;
	private Vector3 RotateAmount;

	private void Awake()
	{
		Instance = this;
	}

	void Update(){

		if (CalibComplete == false) {
			float Spd = MoveSpeed * Time.deltaTime; //conversion of move speed from unit/sec to a coordinate
			RotateAmount = new Vector3 (Spd, Spd, Spd) * 3;
			transform.Rotate (RotateAmount);
			//movement in z axis (forwards and backwards)
			if (Input.GetKey (KeyCode.W))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.position += new Vector3 (0, 0, Spd * 2);
			else
				transform.position += new Vector3 (0, 0, Spd);
			if (Input.GetKey (KeyCode.S))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.position -= new Vector3 (0, 0, Spd * 2);
			else
				transform.position -= new Vector3 (0, 0, Spd);

			//movement in x axis (left and right)
			if (Input.GetKey (KeyCode.A))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.position -= new Vector3 (Spd * 2, 0, 0);
			else
				transform.position -= new Vector3 (Spd, 0, 0);
			if (Input.GetKey (KeyCode.D))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.position += new Vector3 (Spd * 2, 0, 0);
			else
				transform.position += new Vector3 (Spd, 0, 0);

			//movement in y axis (up and down)
			if (Input.GetKey (KeyCode.Q))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.position += new Vector3 (0, Spd * 2, 0);
			else
				transform.position += new Vector3 (0, Spd, 0);
			if (Input.GetKey (KeyCode.E))
			if (Input.GetKey (KeyCode.LeftShift))
				transform.position -= new Vector3 (0, Spd * 2, 0);
			else
				transform.position -= new Vector3 (0, Spd, 0); 
		}
		//complete calibration if space is pressed
		if (Input.GetKey (KeyCode.Space)) {
			CalibComplete = true;
			GetComponent<MeshRenderer> ().enabled = false;
			CalibText.text = "CALIBRATION COMPLETE";
		}
	}
}