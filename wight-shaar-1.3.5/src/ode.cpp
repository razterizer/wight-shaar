#include "ode.h"



/*****************************
 *     Rigid Body Model      *
 *****************************/

State diff_rigid(float t, const State& s_, Vessel *craft,
		 const vector<float>& params) {
  Vector w;                              //b-frame
  Quaternion q;
  Vector tau;                            //b-frame
  Vector F;                              //i-frame
  State s(s_);
  State ds;                              // ds/dt


  w=craft->inertia_inv()*s.ang_mom();     //b-frame

  for(int i=0; i < craft->num_thrusters(); i++){
    F += s.rot() * craft->thruster(i).force(); //i-frame
    tau += craft->thruster(i).torque();        //b-frame
  }


  //The state equation
  ds.pos()     = s.vel();                   //i-frame (ok)
  ds.vel()     = F/craft->mass();           //i-frame (ok)
  ds.rot()     = s.rot().dot(w,CS_BODY);    //i-frame
  ds.ang_mom() = tau - (w ^ s.ang_mom());   //b-frame (ok)


  return ds;
}

/*****************************
 *        Spring Model       *
 *****************************/

//params = {k, c, p_x, p_y, p_z}
//k: spring constant
//c: damping coefficient
//p_x: x displacement centre
//p_y: y displacement centre
//p_z: z displacement centre
State diff_spring(float t, const State& s_, Vessel *craft,
		  const vector<float>& params)
  throw(invalid_argument)
{
  if (params.size()!=5)
    throw invalid_argument("diff_spring(...)");

  State s(s_);
  float k=params[0],
    c=params[1],
    p_x=params[2],
    p_y=params[3],
    p_z=params[4];
  float m=craft->mass();
  State ds;

  Vector x = s.pos()-Vector(p_x, p_y, p_z);  //displacement

  ds.pos() = s.vel();
  ds.vel() = -k/m*x - c/m*s.vel();

  //cout << ds.pos() << endl;

  return ds;
}


/*********************************************
 *                   ODE                     *
 *********************************************/

State& ODE :: step(float dt) {
  this->dt=dt;
  return step();
}

State& ODE :: step(vector<float> p) {
  params=p;
  return step();
}

State& ODE :: step(vector<float> p, float dt) {
  params=p;
  return step(dt);
}

State& ODE :: current(void) const { return craft->state();}

float ODE :: time(void) const { return t;}

float ODE :: timestep(void) const { return dt;}

/*********************************************
 *                  Euler                    *
 *********************************************/

State& Euler :: step(void) {
  if (craft->is_active()) {
    try {
      craft->state() += diff_state(t, craft->state(), craft, params)*dt;
      t += dt;
      craft->state().rot().normalize();
    }
    catch (overflow_error){
      cerr << "division by zero!" << endl;
    }
  }
  return craft->state();
}


/*********************************************
 *               Runge-Kutta                 *
 *********************************************/

State& RungeKutta :: step(void) {
  if (craft->is_active()) {
    try {
      s = craft->state();
      //k1=h*f(t(n),x(n,:));
      k[0]=diff_state(t, s, craft, params)*dt;
      //k2=h*f(t(n)+h/2,x(n,:)+k1/2);
      k[1]=diff_state(t+dt/2, s+(.5*k[0]), craft, params)*dt;
      //k3=h*f(t(n)+h/2,x(n,:)+k2/2);
      k[2]=diff_state(t+dt/2, s+(.5*k[1]), craft, params)*dt;
      //k4=h*f(t(n+1),x(n,:)+k3);
      k[3]=diff_state(t+dt, s+k[2], craft, params)*dt;
      
      //x(n+1,:)=x(n,:)+(k1+2*k2+2*k3+k4)/6;
      craft->state() += (k[0]+2*k[1]+2*k[2]+k[3])*(1.0/6.0);
      t += dt;
      craft->state().rot().normalize();
    }
    catch (overflow_error){
      cerr << "division by zero!" << endl;
    }
  }
  return craft->state();
}

