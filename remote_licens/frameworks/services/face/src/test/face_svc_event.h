void face_event_face_det_send(
	unsigned long long timestamp,
	float *head_pose_ptr,
	unsigned int head_pose_size,
	int score);
void face_event_face_recg_send(
	unsigned long long timestamp,
	char *values_ptr,
	unsigned int values_size,
	float *norm_ptr,
	unsigned int norm_size,
	float score,
	unsigned int max_id);


