struct plane
{
	float A, B, C, D;
	vector3 normal(void);
	
};

struct sphere
{
	vector3 Center;
	float Radius;
};

struct cylinder
{
	vector3 BottomCenter;
	float Height;
	float Radius;
};
