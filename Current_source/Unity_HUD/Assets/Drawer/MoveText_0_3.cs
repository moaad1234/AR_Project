using UnityEngine;
using System.Collections.Generic;

public class MoveText_0_3 : MonoBehaviour {

	private int MoveSpeed_local;
	private GameObject CalibCube = GameObject.Find("CalibCube") ;
	private RotateCube_0_2 RotateCube_0_2_local;


	void Update(){

		//RotateCube_0_2_local = CalibCube.GetComponent<RotateCube_0_2>();
		//MoveSpeed_local = RotateCube_0_2_local.Instance.MoveSpeed;
		MoveSpeed_local = RotateCube_0_2.Instance.MoveSpeed;
		//Debug.Log (MoveSpeed_local);
		float Spd = MoveSpeed_local * Time.deltaTime; //conversion of move speed from unit/sec to a coordinate
		

		//movement in z axis (forwards and backwards)
		if (Input.GetKey (KeyCode.W))
			transform.position += new Vector3(0, 0, Spd);
		if (Input.GetKey (KeyCode.S))
			transform.position -= new Vector3(0, 0, Spd);

		//movement in x axis (left and right)
		if (Input.GetKey (KeyCode.A))
			transform.position -= new Vector3(Spd, 0, 0);
		if (Input.GetKey (KeyCode.D))
			transform.position += new Vector3(Spd, 0, 0);

		//movement in y axis (up and down)
		if (Input.GetKey (KeyCode.Q))
			transform.position += new Vector3(0, Spd, 0);
		if(Input.GetKey(KeyCode.E))
			transform.position -= new Vector3(0, Spd, 0);
	}
}