/*
 * Commands to download memory to file (Download 76800 bytes from address
 * 0x1300000 to file logresp.txt
 * set logfile [open "E:\\Lab_9_Matlab\\log.txt" "w"]
 * puts $logfile [mrd 0x1300000 76800 b]
 * close $logfile
 *
 *
 * */

#include <stdio.h>
#include "xaxidma.h"
#include "define.h"
#include "xdoimgproc.h"
#include "LenaOnCode.h"
#include "AxiTimerHelper.h"

// Get a pointer to the TX and RX dma buffer (CONFIGURE DMA)
// The pointers are for 8-bit memory but their addresses are 32 bit (u32)
unsigned char *m_dma_buffer_TX = (unsigned char *)TX_BUFFER_BASE;
unsigned char *m_dma_buffer_RX = (unsigned char *)RX_BUFFER_BASE;
unsigned char  imgIn_HW[IMG_WIDTH * IMG_HEIGHT];

// Variables
unsigned char imgOut_SW[IMG_WIDTH * IMG_HEIGHT];

// Impulse

/*char kernel[KERNEL_DIM*KERNEL_DIM] = {
                0, 0, 0,
                0, 1, 0,
                0, 0, 0,
   };*/

// Edge

char kernel[KERNEL_DIM * KERNEL_DIM] = {
  -1, -1, -1,
  -1,  8, -1,
  -1, -1, -1,
};

// Use with morphological (Erode, Dilate)

/*char kernel[KERNEL_DIM * KERNEL_DIM] = {
   1, 1, 1,
   1, 1, 1,
   1, 1, 1,
   };*/

XAxiDma axiDma;
int initDMA() {
  XAxiDma_Config *CfgPtr;

  CfgPtr = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
  XAxiDma_CfgInitialize(&axiDma, CfgPtr);

  // Disable interrupts
  XAxiDma_IntrDisable(&axiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
  XAxiDma_IntrDisable(&axiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

  return XST_SUCCESS;
}

XDoimgproc doImgProc;
int initDoImgProc() {
  int status;
  XDoimgproc_Config *doImgProc_cfg;

  doImgProc_cfg = XDoimgproc_LookupConfig(XPAR_DOIMGPROC_0_DEVICE_ID);

  if (!doImgProc_cfg) {
    printf("Error loading config for doHist_cfg\n");
  }
  status = XDoimgproc_CfgInitialize(&doImgProc, doImgProc_cfg);

  if (status != XST_SUCCESS) {
    printf("Error initializing for doHist\n");
  }

  return status;
}

// Do convolution - Software Version
void doImgProcSW(unsigned char *imgIn,
                 unsigned char *imgOut,
                 char           kernel[KERNEL_DIM * KERNEL_DIM]) {
  int   imgsize = IMG_HEIGHT * IMG_WIDTH;
  float kkernel[KERNEL_DIM * KERNEL_DIM];

  for (int i = 0; i < KERNEL_DIM; i++) {
    for (int j = 0; j < KERNEL_DIM; j++) {
      char kernelij = kernel[i * KERNEL_DIM + j];

      if (kernelij < 128) {
        kkernel[i * KERNEL_DIM + j] = kernelij;
      } else kkernel[i * KERNEL_DIM + j] = (int)kernelij - 256;
      kkernel[i * KERNEL_DIM + j] /= 8;
    }
  }

  for (int i = 0; i < IMG_HEIGHT; i++) {
    for (int j = 0; j < IMG_WIDTH; j++) {
      int   outIndex = i * IMG_WIDTH + j;
      float temp     = 0;
      int   kCenter  = -(int)(KERNEL_DIM / 2);

      for (int u = 0; u < KERNEL_DIM; u++) {
        for (int v = 0; v < KERNEL_DIM; v++) {
          int idx = IMG_WIDTH * (i + u + kCenter) + j + v + kCenter;

          if ((0 < idx) && (idx < imgsize)) {
            temp += kkernel[u * KERNEL_DIM + v] * imgIn[idx];
          }
        }
      }
      imgOut[outIndex] = temp;
    }
  }
}

int main() {
  initDMA();
  initDoImgProc();
  AxiTimerHelper axiTimer;

  // Now Doing on software
  printf("Doing convolution on SW\n");
  axiTimer.startTimer();
  doImgProcSW(img, imgOut_SW, kernel);
  axiTimer.stopTimer();
  double SW_elapsed = axiTimer.getElapsedTimerInSeconds();
  printf("Convolution SW execution time: %f sec\n", SW_elapsed);

  // Now doing on hardware
  printf("Doing convolution on HW\n");

  // Populate data (Get image from header and put on memory)
  for (int idx = 0; idx < IMG_WIDTH * IMG_HEIGHT; idx++) {
    imgIn_HW[idx] = img[idx];
  }

  printf("Starting.... HW\n");

  // Ask for a convolution
  XDoimgproc_Write_kernel_Bytes(&doImgProc, 0, kernel, 9);
  printf("Kernel total bytes: %d Bitwidth:%d Base: 0x%X\n",
         XDoimgproc_Get_kernel_TotalBytes(&doImgProc),
         XDoimgproc_Get_kernel_BitWidth(&doImgProc),
         XDoimgproc_Get_kernel_BaseAddress(&doImgProc));
  XDoimgproc_Set_operation(&doImgProc, 0);
  XDoimgproc_Start(&doImgProc);

  // Do the DMA transfer to push and get our image
  axiTimer.startTimer();
  Xil_DCacheFlushRange((u32)imgIn_HW,
                       IMG_WIDTH * IMG_HEIGHT * sizeof(unsigned char));
  Xil_DCacheFlushRange((u32)m_dma_buffer_RX,
                       IMG_WIDTH * IMG_HEIGHT * sizeof(unsigned char));

  XAxiDma_SimpleTransfer(&axiDma,
                         (u32)imgIn_HW,
                         IMG_WIDTH * IMG_HEIGHT * sizeof(unsigned char),
                         XAXIDMA_DMA_TO_DEVICE);
  XAxiDma_SimpleTransfer(&axiDma,
                         (u32)m_dma_buffer_RX,
                         IMG_WIDTH * IMG_HEIGHT * sizeof(unsigned char),
                         XAXIDMA_DEVICE_TO_DMA);

  // Wait transfers to finish
  while (XAxiDma_Busy(&axiDma, XAXIDMA_DMA_TO_DEVICE)) ;

  while (XAxiDma_Busy(&axiDma, XAXIDMA_DEVICE_TO_DMA)) ;

  // Invalidate the cache to avoid reading garbage
  Xil_DCacheInvalidateRange((u32)m_dma_buffer_RX,
                            IMG_WIDTH * IMG_HEIGHT * sizeof(unsigned char));
  axiTimer.stopTimer();

  double HW_elapsed = axiTimer.getElapsedTimerInSeconds();
  printf("HW execution time: %f sec\n", HW_elapsed);

  printf("Time summary SW:%.2fms HW:%.2fms HW was %.2f%% faster\n",
         SW_elapsed * 1000,
         HW_elapsed * 1000,
         (SW_elapsed - HW_elapsed) * 100 / SW_elapsed);
  printf("DMA out addresses are:\n\t0x%X\n\t0x%X\n", m_dma_buffer_RX, imgOut_SW);

  bool imgMismatch = FALSE;

  for (int idxComp = 0; idxComp < (IMG_WIDTH * IMG_HEIGHT); idxComp++) {
    if (imgOut_SW[idxComp] != m_dma_buffer_RX[idxComp]) {
      printf("Invalid response\n");
      imgMismatch = TRUE;
      break;
    }
  }

  if (!imgMismatch) {
    printf("SW and HW images are the same\n");
  }

  return 0;
}
