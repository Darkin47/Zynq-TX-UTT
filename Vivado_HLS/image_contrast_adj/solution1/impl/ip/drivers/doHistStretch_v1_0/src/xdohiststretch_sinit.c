// ==============================================================
// File generated by Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC
// Version: 2016.1
// Copyright (C) 1986-2016 Xilinx, Inc. All Rights Reserved.
// 
// ==============================================================

#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xdohiststretch.h"

extern XDohiststretch_Config XDohiststretch_ConfigTable[];

XDohiststretch_Config *XDohiststretch_LookupConfig(u16 DeviceId) {
	XDohiststretch_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XDOHISTSTRETCH_NUM_INSTANCES; Index++) {
		if (XDohiststretch_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XDohiststretch_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XDohiststretch_Initialize(XDohiststretch *InstancePtr, u16 DeviceId) {
	XDohiststretch_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XDohiststretch_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XDohiststretch_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

