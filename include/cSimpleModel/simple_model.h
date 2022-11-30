#include <stdlib.h>
#include <stdio.h>
#include <fdp/fdp.h>

struct SEIRSModel{
    double S;
    double E;
    double I;
    double R;
    double timesteps;
    double years;
    double alpha;
    double beta;
    double inv_gamma;
    double inv_omega;
    double inv_mu;
    double inv_sigma;
    double *time_data;
    double *S_data;
    double *E_data;
    double *I_data;
    double *R_data;
};

typedef struct SEIRSModel SEIRSModel;

int init_SEIRSModel(SEIRSModel* model, const char* input_path);

void run_SEIRSModel(SEIRSModel* model);

void write_csv_SEIRSModel(SEIRSModel* model, const char* output_path);

void finalise_SEIRSModel(SEIRSModel* model);

void run_local(const char* input_path);

void run_fair();
