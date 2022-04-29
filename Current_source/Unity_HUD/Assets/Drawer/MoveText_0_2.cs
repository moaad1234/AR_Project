using UnityEngine;
using System.Collections.Generic;

public class MoveText_0_2 : MonoBehaviour {
	
	public int MoveSpeed;

	void Update(){

		float Spd = MoveSpeed * Time.deltaTime; //conversion of move speed from unit/sec to a coordinate


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