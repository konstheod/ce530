#include "spice.h"


double calc_exp(struct spec_exp *spec, double timestamp) {
	double value = 0;
	double curr;

	if(timestamp <= spec->td1 ) {
		value = spec->value1;
	}
	else if(timestamp<=spec->td2) {
		value = -(timestamp - spec->td1)/spec->tc1;
		value = spec->value1 + (spec->value2 - spec->value1)*(1 - exp(value));
	} 
	else {
		value = -(timestamp - spec->td2)/spec->tc2;
		curr = -(timestamp - spec->td1)/spec->tc1;
		value = spec->value1 + (spec->value2 - spec->value1)*(exp(value) - exp(curr));
	}
	return value;	
}

double calc_sin(struct spec_sin *spec, double timestamp) {
	double value = 0;
	double curr;

	if(timestamp <= spec->td) {
		
		value =   (2 * M_PI * spec->ph) / 360;
		value = spec->value1 + spec->valuea*(sin(value));
		
	}
	else {
		value = (2 *  M_PI * spec->ph) / 360;
		value += 2 *  M_PI * spec->fr * (timestamp - spec->td);
		curr = -(timestamp - spec->td) * spec->df;
		value = (spec->valuea * sin(value)) * exp(curr);
		value += spec->value1 ;
	}
	return value;	
}

double calc_pulse(struct spec_pulse *spec, double timestamp) {
	double value = 0;

	if (timestamp >= (spec->td + (spec->k + 1) * spec->per)) {
        spec->k++;
    }
	if(timestamp <= spec->td) {
		value = spec->value1;
	}
	else if(timestamp <= (spec->td + spec->tr + spec->k*spec->per)) {

		value = (spec->value2 - spec->value1) / (spec->td + spec->tr + spec->k * spec->per - (spec->td + spec->k * spec->per)) * (timestamp - (spec->td + spec->k * spec->per)) + spec->value1;

	}
	else if(timestamp <= (spec->td + spec->tr + spec->pw + spec->k*spec->per)) {
		value = spec->value2;
	}
	else if(timestamp <= (spec->td + spec->tr + spec->pw + spec->tf + spec->k*spec->per)) {
		value = (spec->value1 - spec->value2) / (spec->td + spec->tr + spec->pw + spec->tf + spec->k * spec->per - (spec->td + spec->tr + spec->pw + spec->k * spec->per)) * (timestamp - (spec->td + spec->tr + spec->pw + spec->k * spec->per)) + spec->value2;
	}
	else {
		value = spec->value1;
	}

	return value;
}

double calc_pwl(struct element *spec, double timestamp) {
	double value = 0;
	int i;

	for(i = 0; i < spec->pwl_spec_len - 1 ; i++) {
		if((timestamp >= spec->pwl_spec[i].t) && (timestamp < spec->pwl_spec[i+1].t)) {

			value = ((spec->pwl_spec[i+1].value - spec->pwl_spec[i].value)/(spec->pwl_spec[i+1].t - spec->pwl_spec[i].t));
			value = value* (timestamp - spec->pwl_spec[i].t) + spec->pwl_spec[i].value;
			break;
		}
	}
	if(i == spec->pwl_spec_len - 1){
		if(timestamp <= spec->pwl_spec[0].t){
			value = spec->pwl_spec[0].value;
		}
		else if(timestamp >= spec->pwl_spec[spec->pwl_spec_len].t) {
			value = spec->pwl_spec[spec->pwl_spec_len].value;
		}
	}
	return value;
}