#include "softmax_layer.h"
#include "blas.h"
#include "dark_cuda.h"
#include "utils.h"
#include "blas.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define SECRET_NUM -1234

void softmax_tree(float *input, int batch, int inputs, float temp, tree *hierarchy, float *output)
{
	int b;
	for (b = 0; b < batch; ++b) {
		int i;
		int count = 0;
		for (i = 0; i < hierarchy->groups; ++i) {
			int group_size = hierarchy->group_size[i];
			softmax(input + b*inputs + count, group_size, temp, output + b*inputs + count, 1);
			count += group_size;
		}
	}
}

softmax_layer make_softmax_layer(int batch, int inputs, int groups, int w, int h, int c)
{
    assert(inputs%groups == 0);
    fprintf(stderr, "softmax                                        %4d\n",  inputs);
    softmax_layer l = { (LAYER_TYPE)0 };
    l.type = SOFTMAX;
    l.w = w;
    l.h = h;
    l.c = c;
    l.batch = batch;
    l.groups = groups;
    l.inputs = inputs;
    l.outputs = inputs;
    l.loss = (float*)xcalloc(inputs * batch, sizeof(float));
    l.output = (float*)xcalloc(inputs * batch, sizeof(float));
    l.delta = (float*)xcalloc(inputs * batch, sizeof(float));
    l.cost = (float*)xcalloc(1, sizeof(float));

    l.forward = forward_softmax_layer;
    l.backward = backward_softmax_layer;
    #ifdef GPU
    l.forward_gpu = forward_softmax_layer_gpu;
    l.backward_gpu = backward_softmax_layer_gpu;

    l.output_gpu = cuda_make_array(l.output, inputs*batch);
    l.loss_gpu = cuda_make_array(l.loss, inputs*batch);
    l.delta_gpu = cuda_make_array(l.delta, inputs*batch);
    #endif
    return l;
}

void forward_softmax_layer(const softmax_layer l, network_state net)
{
    if(l.softmax_tree){
        int i;
        int count = 0;
        for (i = 0; i < l.softmax_tree->groups; ++i) {
            int group_size = l.softmax_tree->group_size[i];
            softmax_cpu(net.input + count, group_size, l.batch, l.inputs, 1, 0, 1, l.temperature, l.output + count);
            count += group_size;
        }
    } else {
        softmax_cpu(net.input, l.inputs/l.groups, l.batch, l.inputs, l.groups, l.inputs/l.groups, 1, l.temperature, l.output);
    }

    #ifdef  CUSTOM_BACKPROP
    for(int j=0; j<l.h; j++)
	for(int i=0; i<l.w; i++)
	{
		int max_idx = -1;
		float max_val = 0;

		for(int k=0; k<l.c; k++)
		{
			int index = (k*l.h*l.w)+(j*l.w)+i;

			if(l.output[index] > max_val)
			{
				max_idx = k;
				max_val = l.output[index];
			}
		}

		for(int k=0; k<l.c; k++)
		{
			float t = (k==max_idx)? 1.0 : 0.0;

			int index = (k*l.h*l.w)+(j*l.w)+i;

			//l.delta[index] = ((k==max_idx)? EPSILON : 0.0);
			l.delta[index] = t - l.output[index];
			//l.delta[index] = l.output[index];
		}
	}
    #else
    if(net.truth && !l.noloss){
        softmax_x_ent_cpu(l.batch*l.inputs, l.output, net.truth, l.delta, l.loss);
        l.cost[0] = sum_array(l.loss, l.batch*l.inputs);
    }
    #endif
}

void backward_softmax_layer(const softmax_layer l, network_state net)
{
    axpy_cpu(l.inputs*l.batch, 1, l.delta, 1, net.delta, 1);
}

#ifdef GPU

void pull_softmax_layer_output(const softmax_layer layer)
{
    cuda_pull_array(layer.output_gpu, layer.output, layer.inputs*layer.batch);
}

#ifdef ORIGINAL_SOFTMAX_GPU
void forward_softmax_layer_gpu(const softmax_layer l, network_state net)
{
    if(l.softmax_tree){
		softmax_tree_gpu(net.input, 1, l.batch, l.inputs, l.temperature, l.output_gpu, *l.softmax_tree);
		/*
		int i;
		int count = 0;
		for (i = 0; i < l.softmax_tree->groups; ++i) {
		int group_size = l.softmax_tree->group_size[i];
		softmax_gpu(net.input_gpu + count, group_size, l.batch, l.inputs, 1, 0, 1, l.temperature, l.output_gpu + count);
		count += group_size;
		}
		*/
    } else {
        if(l.spatial){
			softmax_gpu_new_api(net.input, l.c, l.batch*l.c, l.inputs/l.c, l.w*l.h, 1, l.w*l.h, 1, l.output_gpu);
        }else{
			softmax_gpu_new_api(net.input, l.inputs/l.groups, l.batch, l.inputs, l.groups, l.inputs/l.groups, 1, l.temperature, l.output_gpu);
        }
    }
    if(net.truth && !l.noloss){
        softmax_x_ent_gpu(l.batch*l.inputs, l.output_gpu, net.truth, l.delta_gpu, l.loss_gpu);
        if(l.softmax_tree){
			mask_gpu_new_api(l.batch*l.inputs, l.delta_gpu, SECRET_NUM, net.truth, 0);
			mask_gpu_new_api(l.batch*l.inputs, l.loss_gpu, SECRET_NUM, net.truth, 0);
        }
        cuda_pull_array(l.loss_gpu, l.loss, l.batch*l.inputs);
        l.cost[0] = sum_array(l.loss, l.batch*l.inputs);
    }
}
#else
void forward_softmax_layer_gpu(const softmax_layer l, network_state net)
{
    softmax_instance_folder_gpu(net.input_gpu, l.inputs/l.groups, l.batch, l.inputs, l.c, l.h, l.w, l.temperature, l.output_gpu);
    // softmax_instance_gpu(net.input_gpu, l.inputs/l.groups, l.batch, l.inputs, l.c, l.h, l.w, l.temperature, l.output_gpu);
    // resize the truth from l.w*l.h*1 to l.w*l.h*l.c
    float *truth_temp = calloc(l.c*l.w*l.h, sizeof(float));
    float *truth_temp_gpu = cuda_make_array(truth_temp, l.c*l.w*l.h); 
    int i, j;
    if(net.truth && !l.noloss){
        cuda_pull_array(net.truth_gpu, net.truth, l.batch*l.w*l.h);
        // init truth_temp to 0
        // for (i = 0; i < l.c; ++i){
        //     for (j = 0; j < l.h; ++j){
        //         for (int k = 0; k < l.w; ++k){
        //             int index = i*l.h*l.w+j*l.w+k;
        //             truth_temp[index] = 0;
        //         }
        //     }
        // }
        // set ont-hot map 
        for (i = 0; i < l.h; ++i){
            for (j = 0; j < l.w; ++j){
                int class_id = net.truth[i*l.w+j];
                // printf("truth value is %d\n", class_id);
                int value_index = class_id*l.h*l.w+i*l.w+j;
                truth_temp[value_index] = 1;
            }
        }

        cuda_push_array(truth_temp_gpu, truth_temp, l.batch*l.w*l.h*l.c);
    }
    // calculate delta and loss
    if(net.truth && !l.noloss){
        softmax_x_ent_gpu(l.batch*l.inputs, l.output_gpu, truth_temp_gpu, l.delta_gpu, l.loss_gpu);
        cuda_pull_array(l.loss_gpu, l.loss, l.batch*l.inputs);
        l.cost[0] = sum_array(l.loss, l.batch*l.inputs);
    }
    free(truth_temp);
    cuda_free(truth_temp_gpu);
}
#endif

void backward_softmax_layer_gpu(const softmax_layer layer, network_state net)
{
	axpy_ongpu(layer.batch*layer.inputs, 1, layer.delta_gpu, 1, net.delta, 1);
}

#endif
