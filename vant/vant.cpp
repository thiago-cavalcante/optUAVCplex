#include "UFFLP.h"

#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// CPLEX definitions
#define CPX_PARAM_THREADS  1067
#define CPX_PARAM_EPGAP    2009
#define CPX_PARAM_VARSEL   2028
#define CPX_VARSEL_STRONG  3

int main(int argc, char *argv[])
{
	int ninst, valor;
	int i, j;
	int *ptr_p, *ptr_st;
	FILE *fin, *fout;
	int *proctime, *setup;
	int n, m;
	float start, end;
	double resultado;

	if (argc != 4) {
		printf("\nVANT\n");
		printf("Enter the instance file name, the number of machines and the instance number!\n");
		printf("Example: wet+(#jobs)-(#machines)m-(#instance)  +  #machines  +  #instance\n");
		printf("Output File: JIT+(#jobs)-(#machines)m-(#instance)\n");
		exit(1);
	}
	else if ((fin = fopen(argv[1], "r")) == NULL) {
		printf("SSETBH: unable to open input file! %s\n", argv[1]);
		exit(1);
	}

	m = atoi(argv[2]);      //keep the number of machines
	ninst = atoi(argv[3]);  //keep the instance number

	fscanf(fin, "%d", &n);  //get the number of jobs

	proctime = new int[n];
	setup = new int[n];

	if (proctime == NULL || setup == NULL)
	{
		printf("Scheduling arrays:  Not enough memory\n");
		exit(1);
	}

	printf("Reading instances...\n");
	for (i = 0, ptr_p = proctime, ptr_st = setup; i<n; i++, ptr_p++, ptr_st++)
		fscanf(fin, "%d" "%d", ptr_p, ptr_st);

	fclose(fin);
	printf("Finished reading!\n");

	printf("Processing Times:\n");
	for (i = 0; i<n; i++)
		printf("%d	", proctime[i]);

	printf("\nSetups:\n");
	for (i = 0; i<n; i++)
		printf("%d	", setup[i]);

	start = clock(); //init the execution time

	// create an empty problem instance
	UFFProblem* prob = UFFLP_CreateProblem(UFFLP_Minimize);

	std::string varName;

	puts("Fill the objective function...");

	std::stringstream s;
	s << "C_max";
	s >> varName;
	UFFLP_AddVariable(prob, (char*)varName.c_str(), 0.0, UFFLP_Infinity, 1.0, UFFLP_Integer);

	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			std::stringstream s;
			s << "x_" << i << "_" << j;
			s >> varName;
			UFFLP_AddVariable(prob, (char*)varName.c_str(), 0.0, 1.0, 0.0, UFFLP_Binary);
		}
	}

	std::string consName;
	for (i = 0; i < m; i++)
	{
		// assembly the constraint name "restr1_i"
		std::stringstream s;
		s << "restr1_" << i;
		s >> consName;

		// set the coefficients for the constraint
		for (j = 0; j < n; j++)
		{
			std::stringstream s;
			s << "x_" << i << "_" << j;
			s >> varName;
			UFFLP_SetCoefficient(prob, (char*)consName.c_str(), (char*)varName.c_str(), 1);
		}

		// create the constraint
		UFFLP_AddConstraint(prob, (char*)consName.c_str(), 1, UFFLP_Equal);
	}


	for (i = 0; i < m; i++)
	{
		// assembly the constraint name "restr2_i"
		std::stringstream s;
		s << "restr2_" << i;
		s >> consName;

		// set the coefficients for the constraint
		for (j = 0; j < n; j++)
		{
			// add the variable "x_i_j" to the constraint
			std::stringstream s1;
			s1 << "x_" << i << "_" << j;
			s1 >> varName;
			UFFLP_SetCoefficient(prob, (char*)consName.c_str(), (char*)varName.c_str(), setup[i] + proctime[j]);
		}

		std::stringstream s2;
		s2 << "C_max";
		s2 >> varName;
		UFFLP_SetCoefficient(prob, (char*)consName.c_str(), (char*)varName.c_str(), -1);

		// create the constraint
		UFFLP_AddConstraint(prob, (char*)consName.c_str(), 0, UFFLP_Less);
	}
	//precedencia

	// Write the problem in LP format for debug
	char fname[50];
	sprintf(fname, "vant%d-%dm-%d.lp", n, m, ninst);
	UFFLP_WriteLP(prob, fname);

	// Configure the log file and the log level = 3
	sprintf(fname, "vant%d-%dm-%d.log", n, m, ninst);
	UFFLP_SetLogInfo(prob, fname, 2);

	// solve the problem
	// UFFLP_SetParameter( prob, UFFLP_CutoffValue, bestCost ); // Cutoff value for the objective function
	UFFLP_SetParameter(prob, UFFLP_TimeLimit, 10800); // Maximum number of seconds to run the B&B
	UFFLP_SetCplexParameter(prob, CPX_PARAM_EPGAP, UFFLP_FloatParam, 1e-8);
	UFFLP_SetCplexParameter(prob, CPX_PARAM_VARSEL, UFFLP_IntegerParam, CPX_VARSEL_STRONG);
	UFFLP_SetCplexParameter(prob, CPX_PARAM_THREADS, UFFLP_IntegerParam, 1);
	UFFLP_StatusType status = UFFLP_Solve(prob);

	end = clock();

	// check if an optimal solution has been found
	if (status == UFFLP_Optimal)
	{
		double value;
		std::cout << "Optimal solution found!" << std::endl << std::endl;
		std::cout << "Solution:" << std::endl;

		// get the value of the objective function
		UFFLP_GetObjValue(prob, &value);
		std::cout << "Objective function value = " << value << std::endl;

		// print the total time
		std::cout << "Total Time = " << (end - start) / CLOCKS_PER_SEC << std::endl;

		
		char cname[50];
		for (i = 0; i < m; i++){
			for (j = 0; j < n; j++){
				sprintf(cname, "x_%d_%d", i, j);
				valor = UFFLP_GetSolution(prob, cname, &resultado);
				if (resultado == 1){
					printf("A maquina %ld esta processando a tarefa %ld\n", i, j);
				}
			}
		}


		printf("Saving statistics...\n");
		char fname[50];
		sprintf(fname, "Table_wet%d-%dm.tex", n, m);
		fout = fopen(fname, "at");
		if (fout != NULL){
			fprintf(fout, "% 3d & %8.1lf & %7.1lf \\\\\n",
				ninst,
				value,
				(end - start) / CLOCKS_PER_SEC);
			fclose(fout);
		}
		else
			printf("Could not write to the Latex table file!\n");

	}

	// check if the problem is infeasible
	else if (status == UFFLP_Infeasible)
	{
		std::cout << "The problem is infeasible!" << std::endl;
	}

	// check the other case
	else
	{
		std::cout << "It seems that the solver did not finish its job...";
		std::cout << std::endl;
	}

	// destroy the problem instance
	UFFLP_DestroyProblem(prob);

	delete proctime;
	delete setup;

	return 0;
}
