#include <cSimpleModel/simple_model.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h> // mkdir
#include <sys/types.h>

#define PATH_SIZE 1024
#define LOG_BUFFER_SIZE 4096

int main(int argc, char* argv[]){
    char input_path[PATH_SIZE];
    char output_path[PATH_SIZE];
    if(argc == 1){
        run_fair();
    } else if(argc == 2){
        run_local(argv[1]);
    } else {
        puts(
            "Usage:\n"
            "./simple_model  :  Run using FAIR CLI\n"
            "./simple_model initial_parameters_path  :  Run using provided data file"
        );
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void run_local(const char* input_path){
    int err;
    // create local data_store directory if none exists
    struct stat st = {0};
    if(stat("data_store", &st) == -1){
        mkdir("data_store", 0700);
    }

    SEIRSModel model;
    err = init_SEIRSModel(&model, input_path);
    if(err){
        printf("Error: init_SEIRSModel failed\n");
        exit(EXIT_FAILURE);
    }

    run_SEIRSModel(&model);
    write_csv_SEIRSModel(&model, "data_store/c_simple_model.csv");
    finalise_SEIRSModel(&model);
}

void run_fair(){
    // Get token, config path, script path
    fdp_log(FDP_LOG_INFO, "Reading token");
    char* token = getenv("FDP_LOCAL_TOKEN");
    if(token == NULL){
        fdp_log(FDP_LOG_ERROR, "Error: Could not find env var FDP_LOCAL_TOKEN");
        exit(EXIT_FAILURE);
    }

    char* fdp_path = getenv("FDP_CONFIG_DIR");
    if(fdp_path == NULL){
        fdp_log(FDP_LOG_ERROR, "Error: Could not find env var FDP_CONFIG_DIR");
        exit(EXIT_FAILURE);
    }

    char config_path[PATH_SIZE];
    strcpy(config_path, fdp_path);
    strcpy(config_path + strlen(config_path), "/config.yaml");

    char script_path[PATH_SIZE];
    strcpy(script_path, fdp_path);
    strcpy(script_path + strlen(script_path), "/script.sh");

    char fdp_path_log[LOG_BUFFER_SIZE];
    char config_path_log[LOG_BUFFER_SIZE];
    char script_path_log[LOG_BUFFER_SIZE];
    sprintf(fdp_path_log, "%s: %s", "FDP Path", fdp_path);
    sprintf(config_path_log, "%s: %s", "Config Path", config_path);
    sprintf(script_path_log, "%s: %s", "Script Path", script_path);
    fdp_log(FDP_LOG_INFO, fdp_path_log);
    fdp_log(FDP_LOG_INFO, config_path_log);
    fdp_log(FDP_LOG_INFO, script_path_log);

    // Init FAIR datapipeline
    FdpError err;
    FdpDataPipeline* data_pipeline;
    err = fdp_init(&data_pipeline, config_path, script_path, token);
    if(err){
        char err_string[LOG_BUFFER_SIZE];
        sprintf(err_string, "Error: fdp_init failed, error code %d", (int) err);
        fdp_log(FDP_LOG_ERROR, err_string);
        exit(EXIT_FAILURE);
    }

    // Get/link IO paths
    char input_path[PATH_SIZE];
    err = fdp_link_read(data_pipeline, "SEIRS_model/parameters", input_path, PATH_SIZE);
    if(err){
        char err_string[LOG_BUFFER_SIZE];
        sprintf(err_string, "Error: fdp_link_read failed, error code %d", (int) err);
        fdp_log(FDP_LOG_ERROR, err_string);
        exit(EXIT_FAILURE);
    }

    char output_path[PATH_SIZE];
    err = fdp_link_write(
        data_pipeline, "SEIRS_model/results/model_output/c", output_path, PATH_SIZE
    );
    if(err){
        char err_string[LOG_BUFFER_SIZE];
        sprintf(err_string, "Error: fdp_link_write failed, error code %d", (int) err);
        fdp_log(FDP_LOG_ERROR, err_string);
        exit(EXIT_FAILURE);
    }
    puts(output_path);

    // Run model
    SEIRSModel model;


    fdp_log(FDP_LOG_INFO, "Initialising SEIRS model");
    err = init_SEIRSModel(&model, input_path);
    if(err){
        fdp_log(FDP_LOG_ERROR, "Error: init_SEIRSModel failed");
        exit(EXIT_FAILURE);
    }


    fdp_log(FDP_LOG_INFO, "Running SEIRS model");
    run_SEIRSModel(&model);


    char csv_log_begin[LOG_BUFFER_SIZE] = "Writing SEIRS model to csv ";
    strcpy(csv_log_begin + strlen(csv_log_begin), output_path);
    fdp_log(FDP_LOG_INFO, csv_log_begin);

    write_csv_SEIRSModel(&model, output_path);

    char csv_log_end[LOG_BUFFER_SIZE] = "Successfully written SEIRS model to csv ";
    strcpy(csv_log_end + strlen(csv_log_end), output_path);
    fdp_log(FDP_LOG_INFO, csv_log_end);

    finalise_SEIRSModel(&model);

    err = fdp_finalise(&data_pipeline);
    if(err){
        char err_string[LOG_BUFFER_SIZE];
        sprintf(err_string, "Error: fdp_finalise failed, error code %d", (int) err);
        fdp_log(FDP_LOG_ERROR, err_string);
        exit(EXIT_FAILURE);
    }
}

int init_SEIRSModel(SEIRSModel* model, const char* input_path){
    model->timesteps = 1000;
    model->years = 5;
    model->S =  0.999;
    model->E = 0.001;
    model->I = 0;
    model->R = 0;
    model->time_data = (double*) malloc((1 + (int) model->timesteps) * sizeof(double));
    model->S_data = (double*) malloc((1 + (int) model->timesteps) * sizeof(double));
    model->E_data = (double*) malloc((1 + (int) model->timesteps) * sizeof(double));
    model->I_data = (double*) malloc((1 + (int) model->timesteps) * sizeof(double));
    model->R_data = (double*) malloc((1 + (int) model->timesteps) * sizeof(double));

    // Set parameters from file to default values so we can check for failure.
    double err_val = 1.2345e200;
    model->alpha = err_val;
    model->beta = err_val;
    model->inv_gamma = err_val;
    model->inv_omega = err_val;
    model->inv_mu = err_val;
    model->inv_sigma = err_val;

    FILE* file = fopen(input_path, "r");
    char line[512];
    while(fgets(line, sizeof(line), file)){
        // Remove whitespace
        for(size_t ii=0, jj=0; ii<sizeof(line); ++ii){
            if(isspace(line[ii])) continue;
            line[jj++] = line[ii];
            if(line[ii] == '\0') break;
        }
        // Read key and value
        char key[64];
        double val;
        if(sscanf(line, "%[^,],%lf\n", key, &val) != 2) continue;
        // Add data to model
        if(strstr(key, "alpha")) model->alpha = val;
        if(strstr(key, "beta")) model->beta = val;
        if(strstr(key, "inv_gamma")) model->inv_gamma = val;
        if(strstr(key, "inv_omega")) model->inv_omega = val;
        if(strstr(key, "inv_mu")) model->inv_mu = val;
        if(strstr(key, "inv_sigma")) model->inv_sigma = val;
    }
    fclose(file);

    // Check that all values are set
    if(
        model->alpha == err_val
        || model->beta == err_val
        || model->inv_gamma == err_val
        || model->inv_omega == err_val
        || model->inv_mu == err_val
        || model->inv_sigma == err_val
    ){
        return 1;
    } else {
        return 0;
    }
}

void finalise_SEIRSModel(SEIRSModel* model){
    free(model->time_data);
    free(model->S_data);
    free(model->E_data);
    free(model->I_data);
    free(model->R_data);
}

void run_SEIRSModel(SEIRSModel* model){
        // Prepare Time Units
    double time_unit_years = model->years / model->timesteps;
    double time_unit_days = time_unit_years * 365.25;

    // Convert Parameters to days
    double alpha = model->alpha * time_unit_days;
    double beta = model->beta * time_unit_days;
    double gamma = time_unit_days / model->inv_gamma;
    double omega = time_unit_days / (model->inv_omega * 365.25);
    double mu = time_unit_days / (model->inv_mu * 365.25);
    double sigma = time_unit_days / model->inv_sigma;

    model->time_data[0] = 0;
    model->S_data[0] = model->S;
    model->E_data[0] = model->E;
    model->I_data[0] = model->I;
    model->R_data[0] = model->R;

    for(int ii = 0; ii < model->timesteps; ++ii){
        double N = model->S_data[ii]
            + model->E_data[ii]
            + model->I_data[ii]
            + model->R_data[ii];

        double birth = mu * N;
        double infection = (beta * model->I_data[ii] * model->S_data[ii]) / N;
        double lost_immunity = omega * model->R_data[ii];
        double death_S = mu * model->S_data[ii];
        double death_E = mu * model->E_data[ii];
        double death_I = (mu + alpha) * model->I_data[ii];
        double death_R = mu * model->R_data[ii];
        double latency = sigma * model->E_data[ii];
        double recovery = gamma * model->I_data[ii];

        double S_rate = birth - infection + lost_immunity - death_S;
        double E_rate = infection - latency - death_E;
        double I_rate = latency - recovery - death_I;
        double R_rate = recovery - lost_immunity - death_R;

        int jj = ii + 1;
        double time_point_3dp = ((int)(jj * time_unit_years * 1000 + 0.5)) / 1000.0;
        model->time_data[jj] = time_point_3dp;
        model->S_data[jj] = model->S_data[ii] + S_rate;
        model->E_data[jj] = model->E_data[ii] + E_rate;
        model->I_data[jj] = model->I_data[ii] + I_rate;
        model->R_data[jj] = model->R_data[ii] + R_rate;
    }
}

void write_csv_SEIRSModel(SEIRSModel* model, const char* output_path){
    FILE* file = fopen(output_path, "w");
    fputs("time,S,E,I,R\n", file);
    for(size_t ii=0; ii <= model->timesteps; ++ii){
        fprintf(
            file,
            "%f,%f,%f,%f,%f\n",
            model->time_data[ii],
            model->S_data[ii],
            model->E_data[ii],
            model->I_data[ii],
            model->R_data[ii]
        );
    }
    fclose(file);
}
