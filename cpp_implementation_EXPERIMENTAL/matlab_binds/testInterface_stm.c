#include "helper_input_csv.h"
#include "matlab_wrapper_dtm.h"


#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME  testInterface_stm

#include "simstruc.h"
#include "mex.h"

#ifndef M_PI
    #define M_PI      3.14159265358979323846
#endif

static void mdlInitializeSizes(SimStruct *S){

    //set width of input ports 
    if (!ssSetNumInputPorts(S, 0)) return;


    if (!ssSetNumOutputPorts(S, 9)) return;
    ssSetOutputPortWidth(S, 0, 1);
    ssSetOutputPortWidth(S, 1, 1);
    ssSetOutputPortWidth(S, 2, 1);
    ssSetOutputPortWidth(S, 3, 1);
    ssSetOutputPortWidth(S, 4, 1);
    ssSetOutputPortWidth(S, 5, 1);
    ssSetOutputPortWidth(S, 6, 1);
    ssSetOutputPortWidth(S, 7, 1);
    ssSetOutputPortWidth(S, 8, 1);

    ssSetNumSampleTimes(S, 1);

    //reserve space for the model in the pointer 
    ssSetNumPWork(S, 1);


    ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);


    FILE* datafile;
    char* filepath = "/Users/jens/Desktop/Masterthesis/iac/sim_vehicle_dynamics/cpp_implementation_EXPERIMENTAL/test_scenarios/monte_day4_run5_turn1.csv";
    char line1[500];

    datafile = fopen(filepath, "r");
    
    //step over first line with text
    fgets(line1, sizeof(line1), datafile);

    //skip j lines of file 
    // for(int j=0; j<170;j++){
    //     fgets(line1, sizeof(line1), datafile);
    // }


    char output[500];
    char buffer[500];
    double xpos, ypos, psi, vx, vy, dpsi, omega_fl, omega_fr, omega_rl, omega_rr;
    
    char ini_test_real[300];
    char ini_psi[100];
    char ini_vx[100];
    char ini_vy[100];
    char ini_dpsi[100];
    char ini_omegafl[100];
    char ini_omegafr[100];
    char ini_omegarl[100];
    char ini_omegarr[100];

    fgets(output, sizeof(output), datafile);
    strcpy(buffer, output);

    //TODO check + M_PI 
    // xpos = separate_data(output, 1);
    // strcpy(output, buffer);
    // ypos = separate_data(output, 2);
    // strcpy(output, buffer);
    // psi = separate_data(output, 3);// + M_PI*0.5;
    // strcpy(output, buffer);

    dpsi = separate_data(output, 4);
    strcpy(output, buffer);
    vx = separate_data(output, 5);
    strcpy(output, buffer);
    vy = separate_data(output, 6);
    strcpy(output, buffer);

    omega_fl = separate_data(output, 21);
    strcpy(output, buffer);
    omega_fr = separate_data(output, 22);
    strcpy(output, buffer);
    omega_rl = separate_data(output, 23);
    strcpy(output, buffer);
    omega_rr = separate_data(output, 24);
    strcpy(output, buffer);

    sprintf(ini_test_real, "setValue(x0_stm_init, [%f; %f; %f; %f; %f; %f; %f; %f; %f; %f; %f; %f; %f; %f; %f])", vx, vy, dpsi, omega_fl, omega_fr, omega_rl, omega_rr, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    ssPrintf(ini_test_real);


    //get data dictionary
    mexEvalString("dictObj = Simulink.data.dictionary.open('/Users/jens/Desktop/Masterthesis/iac/sim_vehicle_dynamics/vehicledynamics/datadict/vehicleparameters_vehdynsim.sldd')");
    mexEvalString("dDataSectObj = getSection(dictObj,'Design Data')");

    //set values in data dictionary    
    mexEvalString("x0_stm_init = getEntry(dDataSectObj,'x0_stm_vehiclestates')");
    mexEvalString(ini_test_real);

    fclose(datafile);


}

static void mdlInitializeSampleTimes(SimStruct *S){

    // referenced models containing this S-function can inherit their sample times from their parent model
    ssSetModelReferenceSampleTimeDefaultInheritance(S); 
}


#define MDL_START  
#if defined(MDL_START) 
  static void mdlStart(SimStruct *S)
  {
    Wrapper_dtm *wrapper;

    wrapper = (Wrapper_dtm*)malloc(sizeof(Parameters_dtm)+sizeof(VehicleState));

    wrapper->param_dtm = param_dtm_new();
    wrapper->vehstate = vehstate_new();


    FILE* datafile;
    char* filepath = "/Users/jens/Desktop/Masterthesis/iac/sim_vehicle_dynamics/cpp_implementation_EXPERIMENTAL/test_scenarios/monte_day4_run5_turn1.csv";
    wrapper->duration_csv = 6;
    
    char line1[500];

    datafile = fopen(filepath, "r");
    
    //step over first line with text
    fgets(line1, sizeof(line1), datafile);

    //skip j lines of file 
    // for(int j=0; j<170;j++){
    //     fgets(line1, sizeof(line1), datafile);
    // }

    update_input_data(datafile, wrapper->vehstate);
    wrapper->datafile = datafile;

    ssSetPWorkValue(S, 0, wrapper);

  }                                                  
#endif


static void mdlOutputs(SimStruct *S, int_T tid){

    Wrapper_dtm *wrapper; 
    
    wrapper = ssGetPWorkValue(S, 0);

    Parameters_dtm *param_dtm;
    param_dtm = wrapper->param_dtm;

    VehicleState *vehstate;
    vehstate = wrapper->vehstate;


    //get pointer to output signal
    real_T *torquefl = ssGetOutputPortRealSignal(S, 0);
    real_T *torquefr = ssGetOutputPortRealSignal(S, 1);
    real_T *torquerl = ssGetOutputPortRealSignal(S, 2);
    real_T *torquerr = ssGetOutputPortRealSignal(S, 3);

    real_T *steeringangle   = ssGetOutputPortRealSignal(S, 4);
    
    real_T *brakepressurefl = ssGetOutputPortRealSignal(S, 5);
    real_T *brakepressurefr = ssGetOutputPortRealSignal(S, 6);
    real_T *brakepressurerl = ssGetOutputPortRealSignal(S, 7);
    real_T *brakepressurerr = ssGetOutputPortRealSignal(S, 8);
    
    double stepsize = ssGetStepSize(S);

    if(ssIsMajorTimeStep(S)){
      //timestep t = 0.002 
      wrapper->counter +=stepsize;

      if(wrapper->counter == 0.004 && wrapper->counter != 0){
        //date is stared at a rate of 0.004 seconds -> update only every second step 
        update_input_data(wrapper->datafile, vehstate);
        wrapper->counter = 0;
      } 

    }

    torquefl[0] = vehstate->tirestate_fl.tireDriveTorque;
    torquefr[0] = vehstate->tirestate_fr.tireDriveTorque;
    torquerl[0] = vehstate->tirestate_rl.tireDriveTorque;
    torquerr[0] = vehstate->tirestate_rr.tireDriveTorque;

    steeringangle[0] = vehstate->steeringangle;

    brakepressurefl[0] = vehstate->tirestate_fl.brakePressure_Pa;
    brakepressurefr[0] = vehstate->tirestate_fr.brakePressure_Pa;
    brakepressurerl[0] = vehstate->tirestate_rl.brakePressure_Pa;
    brakepressurerr[0] = vehstate->tirestate_rr.brakePressure_Pa;


    if(ssGetT(S)==wrapper->duration_csv){
      ssSetStopRequested(S, 1);
    }


    UNUSED_ARG(tid); 

}

static void mdlTerminate(SimStruct *S){
    
    Wrapper_dtm *wrapper; 
    wrapper = ssGetPWorkValue(S, 0);

    param_dtm_free(wrapper->param_dtm);
    vehstate_free(wrapper->vehstate);
    free(ssGetPWorkValue(S, 0));
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
