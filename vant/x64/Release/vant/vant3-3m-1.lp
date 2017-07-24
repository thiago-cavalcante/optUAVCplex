\ENCODING=ISO-8859-1
\Problem name: 

Minimize
 obj: C_max
Subject To
 restr1_0: x_0_0 + x_0_1 + x_0_2  = 1
 restr1_1: x_1_0 + x_1_1 + x_1_2  = 1
 restr1_2: x_2_0 + x_2_1 + x_2_2  = 1
 restr2_0: - C_max + 119 x_0_0 + 119 x_0_1 + 125 x_0_2 <= 0
 restr2_1: - C_max + 119 x_1_0 + 119 x_1_1 + 125 x_1_2 <= 0
 restr2_2: - C_max + 119 x_2_0 + 119 x_2_1 + 125 x_2_2 <= 0
Bounds
      C_max >= 0
 0 <= x_0_0 <= 1
 0 <= x_0_1 <= 1
 0 <= x_0_2 <= 1
 0 <= x_1_0 <= 1
 0 <= x_1_1 <= 1
 0 <= x_1_2 <= 1
 0 <= x_2_0 <= 1
 0 <= x_2_1 <= 1
 0 <= x_2_2 <= 1
Binaries
 x_0_0  x_0_1  x_0_2  x_1_0  x_1_1  x_1_2  x_2_0  x_2_1  x_2_2 
Generals
 C_max 
End
