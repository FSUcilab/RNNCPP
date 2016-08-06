#include "activations.h"
#include <stdio.h>

int Activation::counter = 0;

Activation::Activation(std::string name) 
{
	char cname[80];

	if (strlen(cname) > 80) {
		printf("Activation::Activation : cname array too small\n");
		exit(1);
	}
	sprintf(cname, "%s%d", name.c_str(), counter);
	this->name = cname;
	counter++;
}
//----------------------------------------------------------------------
Activation::~Activation() 
{
}
//----------------------------------------------------------------------
void Activation::print(std::string msg)
{
	printf("activation: name= %s\n", this->name.c_str());
	if (msg != "") printf("%s\n", msg.c_str());
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
