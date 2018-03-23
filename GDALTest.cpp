#include<iostream>  
#include"gdal.h"  
#include"gdal_priv.h"  
#include <fstream>
#include<string>
#include<streambuf>
using namespace std;
typedef unsigned char DT_8U;
//对时序影像做差分处理，得到噪声点
void readImage()
{
	char* fileName1 = "g:\\school\\Data\\1990057-L5.img";
	char* fileName2 = "g:\\school\\Data\\1990153-L5.img";
	GDALAllRegister(); 
	GDALDataset *poSrcDS;
	poSrcDS = (GDALDataset*)GDALOpen(fileName1, GA_ReadOnly);
	if (poSrcDS == NULL)
	{
		cout << "open file error!" << endl;
		return;
	}
	int	iXSizeDS = poSrcDS->GetRasterXSize();
	int iYSizeDS = poSrcDS->GetRasterYSize();
	int iBandCountDS = poSrcDS->GetRasterCount();
	GDALDataset *poSrcDF;
	poSrcDF = (GDALDataset*)GDALOpen(fileName2, GA_ReadOnly);
	if (poSrcDF == NULL)
	{
		cout << "open file error!" << endl;
		return;
	}
	int iXSizeDF = poSrcDF->GetRasterXSize();
	int iYSizeDF = poSrcDF->GetRasterYSize();
	int iBandCountDF = poSrcDF->GetRasterCount();

	GDALDriver *poDriver;
	GDALDataset *poDstDS;
	const char*pszFormat = "HFA";
	char *pszDstFile = "g:\\xxx.img";
	poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
	poDstDS = poDriver->Create(pszDstFile, iXSizeDS, iYSizeDS, iBandCountDS, GDT_UInt16, NULL);
	int *pSrcData = new int[iXSizeDS*iYSizeDS];
	int *pDstData = new int[iXSizeDS*iYSizeDS];
	int *pFrcData = new int[iXSizeDS*iYSizeDS];
	if (iXSizeDS == iXSizeDF&&iYSizeDS == iYSizeDF&&iBandCountDS == iBandCountDF)
	{
		for (int iBand = 1; iBand <= iBandCountDS; iBand++)
		{
			GDALRasterBand *pSrcBandDS = poSrcDS->GetRasterBand(iBand);
			GDALRasterBand *pSrcBandDF = poSrcDF->GetRasterBand(iBand);
			GDALRasterBand *pDstBand = poDstDS->GetRasterBand(iBand);
			for (int i = 0; i<iYSizeDS; i++)//循环图像高
			{
				pSrcBandDS->RasterIO(GF_Read, 0, i, iXSizeDS, 1, pSrcData, iXSizeDS, 1, GDT_UInt16, 0, 0);
				pSrcBandDF->RasterIO(GF_Read, 0, i, iXSizeDS, 1, pFrcData, iXSizeDS, 1, GDT_UInt16, 0, 0);
				for (int j = 0; j<iXSizeDS; j++)//循环图像宽
				{
					pDstData[j] = pSrcData[j] - pFrcData[j];
					pDstBand->RasterIO(GF_Write, 0, i, iXSizeDS, 1, pDstData, iXSizeDS, 1, GDT_UInt16, 0, 0);

				}
			}
		}
	}
	else {
		cout << "the image error";
	}

	GDALClose((GDALDatasetH)poSrcDS);
	GDALClose((GDALDatasetH)poSrcDF);
	GDALClose((GDALDatasetH)poDstDS);
	delete[]pSrcData;
	delete[]pFrcData;
	delete[]pDstData;


}
//处理影像一波段，并进行阈值划分，保存该波段信息。
void RST_ReadFile1()
{
	GDALAllRegister();
	GDALDataset* pDTIn = (GDALDataset*)GDALOpen("g:\\school\\Data\\1990057-L5.img", GA_ReadOnly);
	int  nWidth = pDTIn->GetRasterXSize();
	int nHeight = pDTIn->GetRasterYSize();
	int nBandCnt = pDTIn->GetRasterCount();
	unsigned char* pBuf = new unsigned char[nWidth*nHeight];
	pDTIn->GetRasterBand(4)->RasterIO(GF_Read, 0, 0, nWidth, nHeight, pBuf, nWidth, nHeight, GDT_Byte, 0, 0);
	double dMean = 0.0;
	for (int i = 0; i<nWidth*nHeight; i++)
	{
		dMean += pBuf[i];
	}
	dMean /= (double)(nWidth*nHeight);
	cout << dMean;
	GDALDriver* pDriver = (GDALDriver*)GDALGetDriverByName("HFA");
	GDALDataset* pDTRet = (GDALDataset*)GDALCreate(pDriver, "D:\\xxx.img", nWidth, nHeight, 1, GDT_Byte, NULL);
	pDTRet->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, nWidth, nHeight, pBuf, nWidth, nHeight, GDT_Byte, 0, 0);
	double dGeoTrans[6];
	pDTIn->GetGeoTransform(dGeoTrans);
	pDTRet->SetGeoTransform(dGeoTrans);
	const char* pSR = pDTIn->GetProjectionRef();
	pDTRet->SetProjection(pSR);
	GDALClose(pDTRet);
	GDALClose(pDTIn);
	delete[] pBuf;
		
}
//读取影像数据，并记录csv
void WriteDataToCSV()
{
	GDALAllRegister();
	ofstream oFile;
	oFile.open("g:\\scoresheet.csv", ios::out | ios::trunc);
	int a[10000];

	char* filepath1 = "g:\\school\\Data\\1990057-L5.img";
	GDALDataset* pDTIn;
	unsigned char* pBuf;
	pDTIn = (GDALDataset*)GDALOpen(filepath1, GA_ReadOnly);
	int  nWidth = pDTIn->GetRasterXSize();
	int nHeight = pDTIn->GetRasterYSize();
	int nBandCnt = pDTIn->GetRasterCount();
	pBuf = new unsigned char[nWidth*nHeight];
	pDTIn->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, nWidth, nHeight, pBuf, nWidth, nHeight, GDT_Byte, 0, 0);
	for (int i = 0; i<nWidth*nHeight; i++)
	{
		a[i] = pBuf[i];
		printf("%d ", a[i]);
		oFile << a[i] << ",";
	}
	oFile << endl;
	oFile.close();
	GDALClose(pDTIn);
	if (pBuf)
	{
		delete[] pBuf;
		pBuf = NULL;
	}

}