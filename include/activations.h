#ifndef __ACTIVATIONS_H__
#define __ACTIVATIONS_H__

#include <vector>
//#include <math.h>
//#include <Eigen/Core>
#include "typedefs.h"
#include "print_utils.h"


class Activation
{
protected:
	std::string name;
	static int counter;

public:
	Activation(std::string name="activation");
	virtual ~Activation();
	Activation(const Activation&); 
	const Activation& operator=(const Activation&); 
	/** Derivative f'(x) of activation function f(x) */
	/** x has dimensionality equal to the previous layer size */
	/** the return value has the dimensionality of the new layer size */
	virtual VF2D_F derivative(const VF2D_F& x) = 0; // derivative of activation function evaluated at x
	//virtual VF2D_F jacobian(const VF1D_F& x) { ; // difrent variables are coupled, Jacobian
	//	return VF2D_F(1);
	//}
	virtual VF1D derivative(const VF1D& x) = 0; // derivative of activation function evaluated at x
	virtual VF2D_F operator()(const VF2D_F& x) = 0;
	virtual void print(std::string name= "");
	virtual std::string getName() { return name; }
};

//----------------------------------------------------------------------
class Identity : public Activation
{
public:
	Identity(std::string name="Identity") : Activation(name) {;}
	~Identity();
    Identity(const Identity&);
    const Identity& operator=(const Identity&);
 
	VF2D_F operator()(const VF2D_F& x) {
		return x;
	}

	VF2D_F derivative(const VF2D_F& x)
	{
		VF2D_F y(x);
		for (int b=0; b < x.size(); b++) {
			y[b].ones();
		}
		return y;
	}

	VF1D derivative(const VF1D& x)
	{
		//printf("...derivative ...------------------\n");
		VF1D y(size(x));
		//U::print(x, "x");
		//U::print(y, "y");
		for (int b=0; b < x.size(); b++) {
			y[b] = 1.;
		}
		//y.print("return y");
		return y;
	}
};
//----------------------------------------------------------------------
class Tanh : public Activation
{
public:
	Tanh(std::string name="tanh") : Activation(name) {;}
	~Tanh();
    Tanh(const Tanh&);
    const Tanh& operator=(const Tanh&);
 
	VF2D_F operator()(const VF2D_F& x) {
#ifdef ARMADILLO
		VF2D_F y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) {
			y[i] = tanh(x[i]);
		}
		return y;
#else
		AF ex = x;
		ex = (2.*ex).exp(); //exp(x);
		return (ex-1.) / (ex + 1.);
#endif
	}

	VF2D_F derivative(const VF2D_F& x)
	{
#ifdef ARMADILLO
		VF2D_F y(x.n_rows);
		//x.print("***> input to activation (tanh): x");
		for (int i=0; i < x.n_rows; i++) {
			y[i] = tanh(x[i]);
			y[i] = 1.-y[i]%y[i];
		}
		printf("***> tanh= %f, deriv= %f\n", tanh(x[0](0,0)), y[0](0,0));
		return y;
#else
		VF s = this->operator()(x);
		return (1.-s*s);
#endif
	}

	VF1D derivative(const VF1D& x)
	{
		VF1D y(x.n_rows);
		//x.print("***> input to activation (tanh): x");
		for (int i=0; i < x.n_rows; i++) {
			y[i] = tanh(x[i]);
			y[i] = 1.-y[i]*y[i];
		}
		return y;
	}
};


//----------------------------------------------------------------------
class Sigmoid : public Activation
{
public:
	Sigmoid(std::string name="sigmoid") : Activation(name) {;}
	~Sigmoid();
    Sigmoid(const Sigmoid&);
    const Sigmoid& operator=(const Sigmoid&);

	VF2D_F operator()(const VF2D_F& x) {
#ifdef ARMADILLO
		VF2D_F y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) { // loop over field elements
			y[i] = 1. / (1. + exp(-x[i]));
		}
		return y;
#else
		AF ex = x;
		return 1. / (1. + (-ex).exp());
#endif
	}

	//f = 1 / (1 + exp(-x)) = 1/D
	//f' = -1/D^2 * (-exp(-x)-1 + 1) = -1/D^2 * (-D + 1) = 1/D - 1/D^2 = f (1-f)
	VF2D_F derivative(const VF2D_F& x) 
	{
#ifdef ARMADILLO
		VF2D_F y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) {
			y[i] = x[i]%(1.-x[i]);
		}
		return y;
#else
		AF s = this->operator()(x);
		return s%(1-s);
#endif
	}

	VF1D derivative(const VF1D& x) 
	{
		VF1D y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) {
			y[i] = x[i]*(1.-x[i]);
		}
		return y;
	}
};
//----------------------------------------------------------------------
class ReLU : public Activation
{
public:
	ReLU(std::string name="relu") : Activation(name) {;}
	~ReLU();
    ReLU(const ReLU&);
    const ReLU& operator=(const ReLU&);

	VF2D_F operator()(const VF2D_F& x) {
		VF2D_F y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) {
			y[i] = arma::clamp(x[i], 0., x[i].max()); 
		}
		return y;
	}

	//f = 1 / (1 + exp(-x)) = 1/D
	//f' = -1/D^2 * (-exp(-x)-1 + 1) = -1/D^2 * (-D + 1) = 1/D - 1/D^2 = f (1-f)
	VF2D_F derivative(const VF2D_F& x)
	{
		VF2D_F y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) {
			y[i] = arma::zeros<VF2D>(arma::size(x[i]));
			y[i].elem(find(x[i] > 0.)).ones(); 
		}
		return y;
	}

	VF1D derivative(const VF1D& x) 
	{
		VF1D y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) {
			// Use >= or > 
			y[i] = x[i] >= 0. ? 1.0 : 0.0;
		}
		return y;
	}
};
//----------------------------------------------------------------------
class Softmax : public Activation
{
// softmax has n inputs, and n outputs: 
// softmax(x,y) = exp(x)/(exp(x)+exp(y)) ,   exp(y)/(exp(x)+exp(y))
public:
	Softmax(std::string name="softmax") : Activation(name) {;}
	~Softmax();
    Softmax(const Softmax&);
    const Softmax& operator=(const Softmax&);


	VF2D_F operator()(const VF2D_F& x) {
		VF2D_F y(x);
		// softmax over the dimension index of VF2D (first index)
		for (int b=0; b < x.n_rows; b++) {
			float mx = arma::max(arma::max(x[b]));
		    for (int s=0; s < x[b].n_cols; s++) {
			    y(b).col(s) = arma::exp(y(b).col(s)-mx);
				// trick to avoid overflows
				float ssum = 1. / arma::sum(x[b].col(s)); // = arma::exp(y[b]);
				y[b].col(s) = x[b].col(s) * ssum;  // % is elementwise multiplication (arma)
			}
		}
		return y;
	}

	//f = 1 / (1 + exp(-x)) = 1/D
	//f' = -1/D^2 * (-exp(-x)-1 + 1) = -1/D^2 * (-D + 1) = 1/D - 1/D^2 = f (1-f)
	VF2D_F derivative(const VF2D_F& x)
	{
		VF2D_F y(x.n_rows);
		for (int b=0; b < x.n_rows; b++) {
			y[b] = arma::zeros<VF2D>(arma::size(x[b]));
			y[b].elem(find(x[b] > 0.)).ones(); 
		}
		return y;
	}

	VF1D derivative(const VF1D& x) 
	{
		VF1D y(x.n_rows);
		for (int i=0; i < x.n_rows; i++) {
			// Use >= or > 
			y[i] = x[i] >= 0. ? 1.0 : 0.0;
		}
		return y;
	}
};
//----------------------------------------------------------------------


#endif
