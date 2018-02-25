//
// Created by xaq on 10/27/17.
//

#ifndef CRITI_SAMPLE_METHOD_H
#define CRITI_SAMPLE_METHOD_H

#ifdef __cplusplus
extern "C" {
#endif

double sample_maxwell_slave(double T);

double sample_maxwell_host(double T);

double sample_watt(double a, double b);

#ifdef __cplusplus
}
#endif

#endif //CRITI_SAMPLE_METHOD_H
