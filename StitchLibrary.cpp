#include "StitchMeshNode.h"

MStatus
StitchMeshNode::CreateStitchLibrary(void)
{
	//----------------------------------------------------------------------------------------------------------//
	// "X" Stitch -- DONE																						//
	//----------------------------------------------------------------------------------------------------------//
	
	// create cage vertices for stitch "x"
	MPointArray x_cage;
	x_cage.append(-3.000345, 0.0000,  2.569745);
	x_cage.append( 3.000341, 0.0000,  2.565636);
	x_cage.append(-0.003557, 0.0000, -2.629054);

	// create stitch "x" from cage
	Stitch x_stitch(x_cage);

	// create each of x_stitch's 2 yarns
	YarnCurve x_curve0(x_cage);
	x_curve0.addCV(-1.0005100, 0.000,  2.5696190); 
	x_curve0.addCV(-1.0005100, 0.000,  2.5696190);
	x_curve0.addCV(-0.0771385, 0.000,  0.7348210);
	x_curve0.addCV(-1.5015400, 0.000, -0.0228395);
	x_curve0.addKnot(0); x_curve0.addKnot(0); x_curve0.addKnot(0);
	x_curve0.addKnot(1); x_curve0.addKnot(1); x_curve0.addKnot(1);
	x_stitch.addYarnCurve(x_curve0);
	
	YarnCurve x_curve1(x_cage);
	x_curve1.addCV( 1.0005100, 0.000,  2.5696190); 
	x_curve1.addCV( 1.0005100, 0.000,  2.5696190);
	x_curve1.addCV( 0.0771385, 0.000,  0.7348210);
	x_curve1.addCV( 1.5015400, 0.000, -0.0228395);
	x_curve1.addKnot(0); x_curve1.addKnot(0); x_curve1.addKnot(0);
	x_curve1.addKnot(1); x_curve1.addKnot(1); x_curve1.addKnot(1);
	x_stitch.addYarnCurve(x_curve1);
	
	stitches.push_back(x_stitch);

	//----------------------------------------------------------------------------------------------------------//
	// "Y1" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
		
	// create cage vertices for stitch "y1"
	MPointArray y1_cage; 
	y1_cage.append(-27.009586, 0.0469164, -14.236634);
	y1_cage.append(-30.006371, 0.0469164, -9.0378370); 
	y1_cage.append(-24.005688, 0.0469164, -9.0419450);
	
	// create stitch "y1" from cage
	Stitch y1_stitch(y1_cage);

	// create each of y1_stitch's 2 yarns
	YarnCurve y1_curve0(y1_cage);
	y1_curve0.addCV(-28.000374,  0.0000000/5.75, -9.0397570);
	y1_curve0.addCV(-28.000374,  0.0000000/5.75, -9.0397570);
	y1_curve0.addCV(-27.903319,  0.2467020/5.75, -10.596938);
	y1_curve0.addCV(-27.584376,  0.0000000/5.75, -10.811406);
	y1_curve0.addCV(-27.265433, -0.2467020/5.75, -11.025875);
	y1_curve0.addCV(-26.994707, -0.3080930/5.75, -11.236117);
	y1_curve0.addCV(-26.994707, -0.3080930/5.75, -11.236117);
	y1_curve0.addCV(-26.994707, -0.3080930/5.75, -11.236117);
	y1_curve0.addCV(-26.444822, -0.0946891/5.75, -11.458301);
	y1_curve0.addCV(-26.390205,  0.0000000/5.75, -11.475787);
	y1_curve0.addCV(-26.335587,  0.0946891/5.75, -11.493272);
	y1_curve0.addCV(-25.559726,  0.0000000/5.75, -11.627255);
	y1_curve0.addCV(-25.507268,  0.0000000/5.75, -11.635998); 

	y1_curve0.addKnot(0); y1_curve0.addKnot(0); y1_curve0.addKnot(0);
	y1_curve0.addKnot(1); y1_curve0.addKnot(1); y1_curve0.addKnot(1);
	y1_curve0.addKnot(2); y1_curve0.addKnot(2); y1_curve0.addKnot(2);
	y1_curve0.addKnot(3); y1_curve0.addKnot(3); y1_curve0.addKnot(3);
	y1_curve0.addKnot(4); y1_curve0.addKnot(4); y1_curve0.addKnot(4);
	y1_stitch.addYarnCurve(y1_curve0);
	
	YarnCurve y1_curve1(y1_cage);
	y1_curve1.addCV(-26.002292, 0, -9.0392920);
	y1_curve1.addCV(-26.002292, 0, -9.0392920);
	y1_curve1.addCV(-25.960156, 0, -10.472435);
	y1_curve1.addCV(-26.475439, 0, -10.868916);
	y1_curve1.addCV(-26.990722, 0, -11.265396);
	y1_curve1.addCV(-27.017499, 0, -11.236117);
	y1_curve1.addCV(-27.017499, 0, -11.236117);
	y1_curve1.addCV(-27.017499, 0, -11.236117);
	y1_curve1.addCV(-27.428415, 0, -11.367261);
	y1_curve1.addCV(-27.515844, 0, -11.384747);
	y1_curve1.addCV(-27.603273, 0, -11.402232);
	y1_curve1.addCV(-28.452481, 0, -11.627255);
	y1_curve1.addCV(-28.504938, 0, -11.635998); 

	y1_curve1.addKnot(0); y1_curve1.addKnot(0); y1_curve1.addKnot(0);
	y1_curve1.addKnot(1); y1_curve1.addKnot(1); y1_curve1.addKnot(1);
	y1_curve1.addKnot(2); y1_curve1.addKnot(2); y1_curve1.addKnot(2);
	y1_curve1.addKnot(3); y1_curve1.addKnot(3); y1_curve1.addKnot(3);
	y1_curve1.addKnot(4); y1_curve1.addKnot(4); y1_curve1.addKnot(4);
	y1_stitch.addYarnCurve(y1_curve1);
	
	stitches.push_back(y1_stitch);
	
	//----------------------------------------------------------------------------------------------------------//
	// "S" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//

	// create cage vertices for stitch "s"
	MPointArray s_cage;
	s_cage.append(-45.999956, 0, -1.000044);
	s_cage.append(-40.000044, 0, -1.000044);
	s_cage.append(-40.000044, 0, -6.999956);
	s_cage.append(-45.999956, 0, -6.999956);

	// create stitch "s" from cage
	Stitch s_stitch(s_cage);

	// create each of s_stitch's 3 yarns
	YarnCurve s_curve0(s_cage);
	s_curve0.addCV(-46, 0, -4);
	s_curve0.addCV(-46, 0, -4);
	s_curve0.addCV(-40, 0, -4);
	s_curve0.addCV(-40, 0, -4);
	s_curve0.addKnot(0); s_curve0.addKnot(0); s_curve0.addKnot(0);
	s_curve0.addKnot(1); s_curve0.addKnot(1); s_curve0.addKnot(1);
	s_stitch.addYarnCurve(s_curve0);

	YarnCurve s_curve1(s_cage);
	s_curve1.addCV(-44, 0, -7);
	s_curve1.addCV(-44, 0, -7);
	s_curve1.addCV(-44, 0.197298/5.75, -5);
	s_curve1.addCV(-44, 0.197298/5.75, -5);
	s_curve1.addCV(-44, 0.197298/5.75, -5);
	s_curve1.addCV(-44, 0.417964/5.75, -4);
	s_curve1.addCV(-44, 0.417964/5.75, -4);
	s_curve1.addCV(-44, 0.417964/5.75, -4);
	s_curve1.addCV(-44, 0.197298/5.75, -3);
	s_curve1.addCV(-44, 0.197298/5.75, -3);
	s_curve1.addCV(-44, 0.197298/5.75, -3);
	s_curve1.addCV(-44, 0, -1);
	s_curve1.addCV(-44, 0, -1); 
	s_curve1.addKnot(0); s_curve1.addKnot(0); s_curve1.addKnot(0);
	s_curve1.addKnot(1); s_curve1.addKnot(1); s_curve1.addKnot(1);
	s_curve1.addKnot(2); s_curve1.addKnot(2); s_curve1.addKnot(2);
	s_curve1.addKnot(3); s_curve1.addKnot(3); s_curve1.addKnot(3);
	s_curve1.addKnot(4); s_curve1.addKnot(4); s_curve1.addKnot(4);
	s_stitch.addYarnCurve(s_curve1);

	YarnCurve s_curve2(s_cage);
	s_curve2.addCV(-42, 0, -7);
	s_curve2.addCV(-42, 0, -7);
	s_curve2.addCV(-42, 0.141032/5.75, -5);
	s_curve2.addCV(-42, 0.141032/5.75, -5);
	s_curve2.addCV(-42, 0.141032/5.75, -5);
	s_curve2.addCV(-42, 0.429182/5.75, -4);
	s_curve2.addCV(-42, 0.429182/5.75, -4);
	s_curve2.addCV(-42, 0.429182/5.75, -4);
	s_curve2.addCV(-42, 0.141032/5.75, -3);
	s_curve2.addCV(-42, 0.141032/5.75, -3);
	s_curve2.addCV(-42, 0.141032/5.75, -3);
	s_curve2.addCV(-42, 0, -1);
	s_curve2.addCV(-42, 0, -1);
	s_curve2.addKnot(0); s_curve2.addKnot(0); s_curve2.addKnot(0);
	s_curve2.addKnot(1); s_curve2.addKnot(1); s_curve2.addKnot(1);
	s_curve2.addKnot(2); s_curve2.addKnot(2); s_curve2.addKnot(2);
	s_curve2.addKnot(3); s_curve2.addKnot(3); s_curve2.addKnot(3);
	s_curve2.addKnot(4); s_curve2.addKnot(4); s_curve2.addKnot(4);
	s_stitch.addYarnCurve(s_curve2);

	stitches.push_back(s_stitch);

	//----------------------------------------------------------------------------------------------------------//
	// "K" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//

	// create cage vertices for stitch "k"
	MPointArray k_cage;
	k_cage.append(-37.998667, 0, -0.997133);
	k_cage.append(-31.992933, 0, -0.997133); 
	k_cage.append(-31.992933, 0, -7.002867);
	k_cage.append(-37.998667, 0, -7.002867); 

	// create stitch "k" from cage
	Stitch k_stitch(k_cage);

	// create each of k_stitch's 3 yarns
	YarnCurve k_curve0(k_cage);
	k_curve0.addCV(-35.995800, 0, -1.000000);
	k_curve0.addCV(-35.995800, 0, -1.000000); 
	k_curve0.addCV(-35.941606, 0, -1.986025);
	k_curve0.addCV(-36.495843, 0, -2.457182);
	k_curve0.addCV(-37.050081, 0, -2.928339);
	k_curve0.addCV(-37.103353, 0, -4.981323);
	k_curve0.addCV(-34.984986, 0, -4.96651);
	k_curve0.addCV(-32.866620, 0, -4.951696);
	k_curve0.addCV(-33.148081, 0, -2.848607);
	k_curve0.addCV(-33.503611, 0, -2.462986);
	k_curve0.addCV(-33.859141, 0, -2.077364);
	k_curve0.addCV(-33.998615, 0, -1.900063);
	k_curve0.addCV(-33.992465, 0, -0.981611); 
	k_curve0.addKnot(0); k_curve0.addKnot(0); k_curve0.addKnot(0);
	k_curve0.addKnot(1); k_curve0.addKnot(1); k_curve0.addKnot(1);
	k_curve0.addKnot(2); k_curve0.addKnot(2); k_curve0.addKnot(2);
	k_curve0.addKnot(3); k_curve0.addKnot(3); k_curve0.addKnot(3);
	k_curve0.addKnot(4); k_curve0.addKnot(4); k_curve0.addKnot(4);
	k_stitch.addYarnCurve(k_curve0);
	
	YarnCurve k_curve1(k_cage);
	k_curve1.addCV(-36.007762,  0.000000/5.75, -7.003160);
	k_curve1.addCV(-36.007762,  0.000000/5.75, -7.003160); 
	k_curve1.addCV(-35.919763,  0.224330/5.75, -5.245585);
	k_curve1.addCV(-35.919763,  0.224330/5.75, -5.245585);
	k_curve1.addCV(-35.919763,  0.224330/5.75, -5.245585);
	k_curve1.addCV(-35.890083,  0.442729/5.75, -4.777894);
	k_curve1.addCV(-35.890083,  0.442729/5.75, -4.777894);
	k_curve1.addCV(-35.890083,  0.442729/5.75, -4.777894);
	k_curve1.addCV(-35.826345,  0.150185/5.75, -4.296069);
	k_curve1.addCV(-35.925862,  0.000000/5.75, -4.149342);
	k_curve1.addCV(-36.025380, -0.150185/5.75, -4.002616);
	k_curve1.addCV(-36.250393, -0.196831/5.75, -4.019920);
	k_curve1.addCV(-36.250393, -0.196831/5.75, -4.019920);
	k_curve1.addCV(-36.250393, -0.196831/5.75, -4.019920);
	k_curve1.addCV(-36.553017, -0.415696/5.75, -4.016749);
	k_curve1.addCV(-36.672232, -0.415696/5.75, -4.007579);
	k_curve1.addCV(-36.791448, -0.415696/5.75, -3.998408);
	k_curve1.addCV(-37.167435, -0.206826/5.75, -3.980067);
	k_curve1.addCV(-37.167435, -0.206826/5.75, -3.980067);
	k_curve1.addCV(-37.167435, -0.206826/5.75, -3.980067);
	k_curve1.addCV(-38.008975,  0.000000/5.75, -3.999443);
	k_curve1.addCV(-38.008975,  0.000000/5.75, -3.999443);
	k_curve1.addKnot(0); k_curve1.addKnot(0); k_curve1.addKnot(0);
	k_curve1.addKnot(1); k_curve1.addKnot(1); k_curve1.addKnot(1);
	k_curve1.addKnot(2); k_curve1.addKnot(2); k_curve1.addKnot(2);
	k_curve1.addKnot(3); k_curve1.addKnot(3); k_curve1.addKnot(3);
	k_curve1.addKnot(4); k_curve1.addKnot(4); k_curve1.addKnot(4);
	k_curve1.addKnot(5); k_curve1.addKnot(5); k_curve1.addKnot(5);
	k_curve1.addKnot(6); k_curve1.addKnot(6); k_curve1.addKnot(6);
	k_curve1.addKnot(7); k_curve1.addKnot(7); k_curve1.addKnot(7);
	k_stitch.addYarnCurve(k_curve1);
	
	YarnCurve k_curve2(k_cage);
	k_curve2.addCV(-34.000148,  0.000000/5.75, -7.003160);
	k_curve2.addCV(-34.000148,  0.000000/5.75, -7.003160);
	k_curve2.addCV(-34.088146,  0.224330/5.75, -5.245585);
	k_curve2.addCV(-34.088146,  0.224330/5.75, -5.245585);
	k_curve2.addCV(-34.088146,  0.224330/5.75, -5.245585);
	k_curve2.addCV(-34.117826,  0.442729/5.75, -4.777894);
	k_curve2.addCV(-34.117826,  0.442729/5.75, -4.777894);
	k_curve2.addCV(-34.117826,  0.442729/5.75, -4.777894);
	k_curve2.addCV(-34.181564,  0.150185/5.75, -4.296069);
	k_curve2.addCV(-34.082047,  0.000000/5.75, -4.149342);
	k_curve2.addCV(-33.982529, -0.150185/5.75, -4.002616);
	k_curve2.addCV(-33.757516, -0.196831/5.75, -4.019920);
	k_curve2.addCV(-33.757516, -0.196831/5.75, -4.019920);
	k_curve2.addCV(-33.757516, -0.196831/5.75, -4.019920);
	k_curve2.addCV(-33.454892, -0.415696/5.75, -4.016749);
	k_curve2.addCV(-33.335677, -0.415696/5.75, -4.007579);
	k_curve2.addCV(-33.216461, -0.415696/5.75, -3.998408);
	k_curve2.addCV(-32.840474, -0.206826/5.75, -3.980067);
	k_curve2.addCV(-32.840474, -0.206826/5.75, -3.980067);
	k_curve2.addCV(-32.840474, -0.206826/5.75, -3.980067);
	k_curve2.addCV(-31.998934,  0.000000/5.75, -3.999443);
	k_curve2.addCV(-31.998934,  0.000000/5.75, -3.999443);
	k_curve2.addKnot(0); k_curve2.addKnot(0); k_curve2.addKnot(0);
	k_curve2.addKnot(1); k_curve2.addKnot(1); k_curve2.addKnot(1);
	k_curve2.addKnot(2); k_curve2.addKnot(2); k_curve2.addKnot(2);
	k_curve2.addKnot(3); k_curve2.addKnot(3); k_curve2.addKnot(3);
	k_curve2.addKnot(4); k_curve2.addKnot(4); k_curve2.addKnot(4);
	k_curve2.addKnot(5); k_curve2.addKnot(5); k_curve2.addKnot(5);
	k_curve2.addKnot(6); k_curve2.addKnot(6); k_curve2.addKnot(6);
	k_curve2.addKnot(7); k_curve2.addKnot(7); k_curve2.addKnot(7);
	k_stitch.addYarnCurve(k_curve2);

	stitches.push_back(k_stitch);

	//----------------------------------------------------------------------------------------------------------//
	// "P" Stitch -- DONE																						//
	//----------------------------------------------------------------------------------------------------------//
	
	// create cage vertices for stitch "p"
	MPointArray p_cage;
	p_cage.append(-0.5, 0.0, -0.5);
	p_cage.append(-0.5, 0.0,  0.5);
	p_cage.append( 0.5, 0.0,  0.5);
	p_cage.append( 0.5, 0.0, -0.5);

	// create stitch "p" from cage
	Stitch p_stitch(p_cage);

	// create each of p_stitch's 3 yarns
	YarnCurve p_curve0(p_cage);
	p_curve0.addCV( 0.500000000,  0.00000000, -0.167040);
	p_curve0.addCV( 0.500000000,  0.00000000, -0.167040);
	p_curve0.addCV( 0.207771000, -0.03185650, -0.152330);
	p_curve0.addCV( 0.207771000, -0.03185650, -0.152330);
	p_curve0.addCV( 0.207771000, -0.03185650, -0.152330);
	p_curve0.addCV( 0.129589000, -0.07489700, -0.147368);
	p_curve0.addCV( 0.129589000, -0.07489700, -0.147368);
	p_curve0.addCV( 0.129589000, -0.07489700, -0.147368);
	p_curve0.addCV( 0.049043700, -0.04998820, -0.136713);
	p_curve0.addCV( 0.024515900, -0.02760550, -0.153349);
	p_curve0.addCV(-1.1953e-005, -0.00522291, -0.169985);
	p_curve0.addCV( 0.002880830,  0.01795180, -0.207600);
	p_curve0.addCV( 0.002880830,  0.01795180, -0.207600);
	p_curve0.addCV( 0.002880830,  0.01795180, -0.207600);
	p_curve0.addCV( 0.002350680,  0.07519160, -0.258189);
	p_curve0.addCV( 0.000817694,  0.07519160, -0.278117);
	p_curve0.addCV(-0.000715295,  0.07519160, -0.298046);
	p_curve0.addCV(-0.003781270,  0.03121760, -0.360899);
	p_curve0.addCV(-0.003781270,  0.03121760, -0.360899);
	p_curve0.addCV(-0.003781270,  0.03121760, -0.360899);
	p_curve0.addCV(-0.000000000,  0.00000000, -0.500000);
	p_curve0.addCV(-0.000000000,  0.00000000, -0.500000);
	p_curve0.addKnot(0); p_curve0.addKnot(0); p_curve0.addKnot(0);
	p_curve0.addKnot(1); p_curve0.addKnot(1); p_curve0.addKnot(1);
	p_curve0.addKnot(2); p_curve0.addKnot(2); p_curve0.addKnot(2);
	p_curve0.addKnot(3); p_curve0.addKnot(3); p_curve0.addKnot(3);
	p_curve0.addKnot(4); p_curve0.addKnot(4); p_curve0.addKnot(4);
	p_curve0.addKnot(5); p_curve0.addKnot(5); p_curve0.addKnot(5);
	p_curve0.addKnot(6); p_curve0.addKnot(6); p_curve0.addKnot(6);
	p_curve0.addKnot(7); p_curve0.addKnot(7); p_curve0.addKnot(7);
	p_stitch.addYarnCurve(p_curve0);
	
	YarnCurve p_curve1(p_cage);
	p_curve1.addCV( 0.5000000000,  0.00000000, 0.167040);
	p_curve1.addCV( 0.5000000000,  0.00000000, 0.167040);
	p_curve1.addCV( 0.2077710000, -0.03185650, 0.152329);
	p_curve1.addCV( 0.2077710000, -0.03185650, 0.152329);
	p_curve1.addCV( 0.2077710000, -0.03185650, 0.152329);
	p_curve1.addCV( 0.1295890000, -0.07489700, 0.147368);
	p_curve1.addCV( 0.1295890000, -0.07489700, 0.147368);
	p_curve1.addCV( 0.1295890000, -0.07489700, 0.147368);
	p_curve1.addCV( 0.0490437000, -0.04998820, 0.136713);
	p_curve1.addCV( 0.0245159000, -0.02760550, 0.153349);
	p_curve1.addCV(-1.19531e-005, -0.00522291, 0.169985);
	p_curve1.addCV( 0.0028808300,  0.01795180, 0.207600);
	p_curve1.addCV( 0.0028808300,  0.01795180, 0.207600);
	p_curve1.addCV( 0.0028808300,  0.01795180, 0.207600);
	p_curve1.addCV( 0.0023506800,  0.07519160, 0.258188);
	p_curve1.addCV( 0.0008176940,  0.07519160, 0.278117);
	p_curve1.addCV(-0.0007152950,  0.07519160, 0.298046);
	p_curve1.addCV(-0.0037812700,  0.03121760, 0.360898);
	p_curve1.addCV(-0.0037812700,  0.03121760, 0.360898);
	p_curve1.addCV(-0.0037812700,  0.03121760, 0.360898);
	p_curve1.addCV(-0.0000000000,  0.00000000, 0.500000);
	p_curve1.addCV(-0.0000000000,  0.00000000, 0.500000);
	p_curve1.addKnot(0); p_curve1.addKnot(0); p_curve1.addKnot(0);
	p_curve1.addKnot(1); p_curve1.addKnot(1); p_curve1.addKnot(1);
	p_curve1.addKnot(2); p_curve1.addKnot(2); p_curve1.addKnot(2);
	p_curve1.addKnot(3); p_curve1.addKnot(3); p_curve1.addKnot(3);
	p_curve1.addKnot(4); p_curve1.addKnot(4); p_curve1.addKnot(4);
	p_curve1.addKnot(5); p_curve1.addKnot(5); p_curve1.addKnot(5);
	p_curve1.addKnot(6); p_curve1.addKnot(6); p_curve1.addKnot(6);
	p_curve1.addKnot(7); p_curve1.addKnot(7); p_curve1.addKnot(7);
	p_stitch.addYarnCurve(p_curve1);
	
	YarnCurve p_curve2(p_cage);
	p_curve2.addCV(-0.500000, 0.00, -0.16715500);
	p_curve2.addCV(-0.500000, 0.00, -0.16715500);
	p_curve2.addCV(-0.337119, 0.00, -0.15809600);
	p_curve2.addCV(-0.258357, 0.00, -0.25074600);
	p_curve2.addCV(-0.179595, 0.00, -0.34339600);
	p_curve2.addCV( 0.163595, 0.00, -0.35230100);
	p_curve2.addCV( 0.161119, 0.00,  0.00181916);
	p_curve2.addCV( 0.158643, 0.00,  0.35593900);
	p_curve2.addCV(-0.192924, 0.00,  0.30888800);
	p_curve2.addCV(-0.257387, 0.00,  0.24945600);
	p_curve2.addCV(-0.321850, 0.00,  0.19002300);
	p_curve2.addCV(-0.351489, 0.00,  0.16670800);
	p_curve2.addCV(-0.500000, 0.00,  0.16773600);
	p_curve2.addKnot(0); p_curve2.addKnot(0); p_curve2.addKnot(0);
	p_curve2.addKnot(1); p_curve2.addKnot(1); p_curve2.addKnot(1);
	p_curve2.addKnot(2); p_curve2.addKnot(2); p_curve2.addKnot(2);
	p_curve2.addKnot(3); p_curve2.addKnot(3); p_curve2.addKnot(3);
	p_curve2.addKnot(4); p_curve2.addKnot(4); p_curve2.addKnot(4);
	p_stitch.addYarnCurve(p_curve2);

	stitches.push_back(p_stitch);
	
	//----------------------------------------------------------------------------------------------------------//
	// "SK" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "KP" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "D12K" Stitch -- TODO																					//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "K1Y" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "PY" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "YKY" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "KYK" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "KPK" Stitch -- TODO																						//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "D312P" Stitch -- TODO																					//
	//----------------------------------------------------------------------------------------------------------//
	//----------------------------------------------------------------------------------------------------------//
	// "D123K" Stitch -- TODO																					//
	//----------------------------------------------------------------------------------------------------------//

	return MStatus::kSuccess;
}