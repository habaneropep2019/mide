
#version 3.0
global_settings { assumed_gamma 2.0 }

#include "colors.inc"
#include "textures.inc"
#include "metals.inc"

camera {
  location <15, -15, -130>
  look_at <0, 0, 0>
}

light_source { <50, 50, -50> colour White }
light_source { <0, 0, -100> colour Gray50 }

union {
    cone {
	<15, 0, 0>, 0.0
	<2, 0, 0>, 4.0
	texture { T_Silver_2A }
	pigment { color SlateBlue }
    }
    cylinder {
	<2, 0, 0>,
	<-10, 0, 0>,
	4.0
	texture { T_Silver_2A }
	pigment { color SlateBlue }
    }
    prism {
	linear_sweep
	linear_spline
	0,
	0.5,
	4,
	<11.5, 0>, <-9.5, 10>, <-9.5, -10>, <11.5, 0>
	rotate <90, 0, 0>
	texture { T_Silver_2A }
	pigment { color Red }
    }
    prism {
	linear_sweep
	linear_spline
	-0.5,
	0.5,
	4,
	<9, 0>, <-9.5, 8>, <-9.5, -8>, <9, 0>
	pigment { color White }
    }

    rotate <0, 0,-360*clock>
    scale <5, 5, 5>
}


