## Cirrus SR-22T Simulator Development 
The following document outlines how to load in aircraft specific parameters and simulate the 3-DOF longitudinal dynamics of the vehicle. The simulator was constructed following the process outlined in "Sub-optimal control laws for minimization of take-off distance for aircrafts." This paper, along with "Mathematical modeling for optimization of competition sailplane flight: a preliminary approach", (which corrected some of the variable naming conventions) can be found in the `Resources` folder of this repsitory. 

The simulator currently does not have a propulsion model implemented.

# Aircraft Parameter Generation 
Aircraft specfic parameters are needed to run the simulator. The process of generating an aircraft parameter file can be seen in `cea_308.m` found in `568-Sim-Framework\Aircraft Params`. The naming convention found in this file has been retained and will be used moving foreward. It is critical that this be the case, as it maintains backward compatibility with any paramter file loaded in. A similar file for Cirrus SR-22T (`Cirrrus_SR22T.mat`) parameters is currently under development. These scripts generate a `.mat` file containing a struct where the fields are the relevent parameters to run the sim. 

# Running the simulator 
1. Open `run_3DoF_SIM`. Ensure that the correct parameter file is being loaded into the base workspace. 
2. The intial states are set with `states_init` variable. 
The states are in the following format: 
    `x y theta Vx Vy q`
3. Set the initial control deflections. 
 - `deltaE` variable sets the initial elevator deflection
 - `throttle` sets the throttle setting. 
4. `Takeoff_Model` should launch automatically. 
5. Run the Sim. 
6. Once the Sim is run, plots vizuallizing results should be displayed automatically. 

# Next Steps 
1. Finalize the Cirrus paramter file. 
2. Add logic to programs maneuvers outside of the Simulink model itsef. 
3. Get a propulsion model. The current approach with the propulsion model is to essentially guess and check with the thrust required for level flight at a certain airspeed altitude combination. With an actual engine model, this process would be as easy as setting the engine RPM/throttle. 
4. Perform system identifaction on the Cirrus simulator to evaluate the model quality, adjust paramters as needed to achieve desired system response. 
5. Linearize the model (if needed), and proceed with control development. 




