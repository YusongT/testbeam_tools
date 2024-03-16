#include <typeinfo>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <TMath.h>
#include <TGraphErrors.h>
#include <TGraph2D.h>
#include <TRandom.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TF2.h>
#include <TH1.h>
#include <iomanip>
#include <sstream>
#include <string>

#include "helperFunctions_Yusong.cxx"

using namespace std; 


std::vector<std::vector<double>> parser_eff_tb(std::vector<std::string> theData){
	//printVector(theData);
	std::vector<double> effV;
	std::vector<double> uncV_p;//plus
	std::vector<double> uncV_m;//minus
	std::vector<std::vector<double>> toRet;
	for (int i=0;i<theData.size();i++){
		effV.push_back(atof(findBetween(theData[i],"","(").c_str()));
		uncV_p.push_back(atof(findBetween(theData[i],"+"," ").c_str()));
		uncV_m.push_back(atof(findBetween(theData[i],"-",")").c_str()));
	}
	toRet.push_back(effV);
	toRet.push_back(uncV_p);
	toRet.push_back(uncV_m);
	//printVector(effV,"eff: ");
	//printVector(uncV_p,"uncV_p: ");
	//printVector(uncV_m,"uncV_m: ");
	return toRet;
}


std::vector<std::vector<double>> tb_getYandUncs(std::vector<TString> inF_names, canOptions canOpts, TString folderInRoot){
	std::vector<std::vector<double>> YandUncs={{},{},{}};
	for(int i=0;i<inF_names.size();i++){
		std::cout<<inF_names[i]<<"\n";
		TString fullPath=canOpts.inFolder+inF_names[i].Data();
		TFile* aTFile=TFile::Open(fullPath);

		TEfficiency* p_TEff = new TEfficiency();
		p_TEff=(TEfficiency*)aTFile->Get(folderInRoot);
		double oneEff=0; double unc_l=0; double unc_u=0;
		std::cout<<"1\n";
		oneEff=100*(p_TEff->GetEfficiency(1));
		unc_l=100*(p_TEff->GetEfficiencyErrorLow(1));
		unc_u=100*(p_TEff->GetEfficiencyErrorUp(1));
		std::cout<<"oneEff: "<<oneEff<<"+"<<unc_u<<"-"<<unc_l<<"\n";
		YandUncs[0].push_back(oneEff);
		YandUncs[1].push_back(unc_u);
		YandUncs[2].push_back(unc_l);
		aTFile->Close();
	}
	return YandUncs;
}


void tb_plot_eff(){
	// colours and markers
	std::vector<int> cVec ={kBlack, kRed, kGreen, kBlue, kCyan, kOrange, kOrange-3, kMagenta, kViolet+6, kSpring-6, kAzure-6, kOrange-7, kOrange+3};
	//std::vector<int> cVec ={kBlack, kRed, kGreen, kBlue, kOrange, kMagenta, kCyan, 12, kOrange-3, kSpring-6, kAzure-6, kViolet+1};
	std::vector<int> mVec       ={20, 21, 22, 23, 20, 21, 22, 23, 29, 33, 34, 43, 47};
	//std::vector<int> mVec      ={20, 21, 22, 23, 29, 33, 34, 47};//43 too small
	std::vector<int> mVec_hollow={24, 25, 26, 32, 27, 28, 30, 46};
	//the colours&markers when you one wants to compare two different configurations e.g. IV before and after irradiation, CV at two frequencies
	std::vector<int> cVecComp2={kBlack, kBlack, kRed, kRed, kGreen, kGreen, kBlue, kBlue, kOrange, kOrange, kMagenta, kMagenta, kCyan, kCyan, 12, 12, kOrange-3, kOrange-3, kSpring-6, kSpring-6, kAzure-6, kAzure-6, kViolet+1, kViolet+1};
	std::vector<int> mVecComp2={20, 24, 21, 25, 22, 26, 23, 32, 29, 30, 33, 27, 34, 28, 47, 46};//43, 42, 
	std::vector<int> mVecComp2_thick1={20, 53, 21, 54, 22, 55, 23, 59, 29, 58, 33, 56, 34, 57, 47, 67};//43, 65, // thickness1
	std::vector<int> mVecComp2_thick2={20, 71, 21, 72, 22, 73, 23, 77, 29, 76, 33, 74, 34, 75, 47, 85};//43, 83, // thickness2
	std::vector<int> mVecComp2_thick3={20, 89, 21, 90, 22, 91, 23, 95, 29, 94, 33, 92, 34, 93, 47, 103};//43, 101, 
	std::vector<int> mVecComp2_thick4={20, 107, 21, 108, 22, 109, 23, 113, 29, 112, 33, 110, 34, 111, 47, 121};//43, 119, 



	canOptions canOpts;
	canOpts.colours=cVec; canOpts.markers=mVec;
	canOpts.setValues(1, -12, 0, 110, "eff_vs_V", "Voltage [V]", "Efficiency [%]", "ap", 900, 600);
	canOpts.figFileName="figFileName";

	canOpts.figFileFormats={"pdf"};//figure file name
	canOpts.inFolder="/Users/ytian/hardware/2021_testBeam/analysis/";
	canOpts.outFolder="./output_tb/";
	std::vector<std::string> fileNameVec={};
	std::vector<std::string> legNameVec={};
	TCanvas* c1; TPad* pad0; TPad* pad1; canOpts.TPads={pad0,pad1};
	std::vector<std::vector<std::vector<std::string>>> allData;
	std::vector<std::vector<std::vector<double>>> allXYvecs;
	std::vector<int> indices;
	TLegend* canLeg=new TLegend(0.72,0.05,1.0,0.95);
	std::vector<double> legendPos={0.72,0.05,1.0,0.95};
	std::vector<TGraphAsymmErrors*> allTGasymms;

	canOpts.plotType="eff";
	canOpts.markerSizes[0]=1.25; canOpts.markerSizes.push_back(1.25);
	canOpts.marginT=0.1; canOpts.marginB=0.1; canOpts.marginL=0.13; canOpts.marginR=0.07; canOpts.xLabelSize=0.035;
	canOpts.axisOffsets={1.05,1.2,1.1};//x, y, z. z so far not used

	canOpts.factorsX={1.}; canOpts.factorsY={1.};
	canOpts.indexRow=0;// index of the row below which data starts
	canOpts.indexColX=1;
	canOpts.indexColY=2;
	//canOpts.indexColXunc=-1;
	//canOpts.indexColYunc=-1;
	std::vector<TString> inF_names={};
	std::vector<double> Xvec={};
	std::vector<std::vector<double>> YandUncs;


	//------ ------ config ------ ------
	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-10/";
	//canOpts.inFolder="/Users/ytian/code/output_tb/";
	canOpts.outFolder="./output_tb/";




	/*
	canOpts.figFileName="2023-9_2023-11_eff_singleRuns_differentROI_2023-11-22";
	canOpts.setValues(15239, 15261, 50, 100, "eff_vs_run", "run#", "eff [%]", "ap", 900, 600);
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_singleRuns_differentROI_2023-11-22/";
	folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_run15240_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15241_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15242_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15243_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15244_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15245_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15246_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15247_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15248_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15251_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15252_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15253_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15254_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15255_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15256_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15257_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15258_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15259_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15260_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_2023-11-22_-32to999.root"
	};
	Xvec={15240,15241,15242,15243,15244,15245,15246,15247,15248,15251, 15252, 15253, 15254, 15255, 15256, 15257, 15258, 15259, 15260};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q16(pl110)_pl24ref"));
	*/




	/*
	canOpts.figFileName="2023-9_2023-11_eff_runs_differentROI";
	canOpts.setValues(15239, 15261, 50, 100, "eff_vs_run", "V", "eff [%]", "ap", 900, 600);
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_singleRuns_differentROI/";
	folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_run15240_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15241_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15242_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15243_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15244_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15245_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15246_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15247_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15248_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15251_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15252_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15253_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15260_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10diffROI_-32to999.root"
	};
	Xvec={15240,15241,15242,15243,15244,15245,15246,15247,15248,15251,15252,15253,15260};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q16(pl110)_pl24ref"));
	*/


	/*
	canOpts.figFileName="2023-9_2023-11_eff_singleRuns";
	canOpts.setValues(15239, 15261, 50, 100, "eff_vs_run", "V", "eff [%]", "ap", 900, 600);
	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_singleRuns/";
	folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_run15240_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		//"analysis_2023-9_15240-15260_noMask_run15241_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		//"analysis_2023-9_15240-15260_noMask_run15242_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15243_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15244_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15245_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15246_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15247_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15248_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_run15260_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
	};
	Xvec={15240,15241,15242,15243,15244,15245,15246,15247,15248,15260};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q16(pl110)_pl24ref"));
	*/

	/*
	canOpts.figFileName="2023-9_2023-11_eff_allComparisons";
	canOpts.setValues(250, 650, 50, 100, "eff_vs_V", "V", "eff [%]", "ap", 900, 600);
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";

	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane131/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
	};
	Xvec={300,400,500};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q11(pl131)_pl24ref"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane131/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root",
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root"
	};
	Xvec={300,400,500};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q11(pl131)_pl24ref_lv1_0to33"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane131/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
	};
	Xvec={300,400,500};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q11(pl131)_pl24ref_previousAlign"));



	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane132/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
	};
	Xvec={300,400,500};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q13(pl132)_pl24ref"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane132/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root",
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root"
	};
	Xvec={300,400,500};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q13(pl132)_pl24ref_lv1_0to33"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane132/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
	};
	Xvec={300,400,500};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q13(pl132)_pl24ref_previousAlign"));



	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_-32to999.root",
	};
	Xvec={400,500,600};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q16(pl110)_pl24ref"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root",
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH10_0to33.root",
	};
	Xvec={400,500,600};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q16(pl110)_pl24ref_lv1_0to33"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2023-9_analysis/analysis_2023-11/";
	folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	inF_names={
		"analysis_2023-9_15273-15299_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
		"analysis_2023-9_15240-15260_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
		"analysis_2023-9_15179-15200_noMask_4Dabs100100_pl24None_minH7_2023-11_ChristopherAlignment_-32to999.root",
	};
	Xvec={400,500,600};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Q16(pl110)_pl24ref_previousAlign"));
	*/



	canOpts.figFileName="2023-11_eff_FEI4_withAndwoMask_newLegend";
	canOpts.setValues(10, 130, 98.5, 100, "eff_vs_V", "Bias voltage [V]", "Efficiency [%]", "ap", 800, 600);
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	canOpts.colours={kBlack, kRed, kBlue, kCyan, kGreen, kOrange, kOrange-3, kMagenta, kViolet+6, kSpring-6, kAzure-6, kOrange-7, kOrange+3};

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-10/";
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_60V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "ref_SCC4"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-10/";
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_60V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "ref_FEI4"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-11_noMask/";
	inF_names={
		"analysis_202211_20V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_40V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_60V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_80V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_100V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH8_-32to999.root",
		"analysis_202211_120V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "ref_SCC4_noMask"));//new align

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-11_noMask/";
	inF_names={
		"analysis_202211_20V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_40V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_60V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_80V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_100V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH8_-32to999.root",
		"analysis_202211_120V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root"
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "ref_FEI4_noMask"));// new align

	//allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "ref_SCC4_noMask_lessAlign"));

	legendPos={0.65,0.1,0.95,0.5};//x1, y1, x2, y2



	/*
	canOpts.figFileName="2023-11_eff_lv1_FEI4_withAndwoMask";
	canOpts.setValues(10, 130, 98.5, 100, "eff_vs_V", "V", "eff [%]", "ap", 900, 600);
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-10/";
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_60V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "SCC4_ref_nolv1Cut"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-10/";
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-1to32.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-1to32.root",
		"analysis_202211_60V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-1to32.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-1to32.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-10_-1to32.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-1to32.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "lv1_-1to32"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-10/";
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_60V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "pl24ref"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-11_noMask/";
	inF_names={
		"analysis_202211_20V2ke_noMask_4Dabs100100_pl130None_minH7_2023-10alignment_DUTalignMinH9_-32to999.root",
		"analysis_202211_40V2ke_noMask_4Dabs100100_pl130None_minH7_2023-10alignment_DUTalignMinH9_-32to999.root",
		"analysis_202211_60V2ke_noMask_4Dabs100100_pl130None_minH7_2023-10alignment_DUTalignMinH9_-32to999.root",
		"analysis_202211_80V2ke_noMask_4Dabs100100_pl130None_minH7_2023-10alignment_DUTalignMinH9_-32to999.root",
		//"analysis_202211_100V2ke_noMask_4Dabs100100_pl130None_minH7_2023-10alignment_DUTalignMinH8_-32to999.root",
		"analysis_202211_120V2ke_noMask_4Dabs100100_pl130None_minH7_2023-10alignment_DUTalignMinH9_-32to999.root"

	};
	Xvec={20,40,60,80,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "no_mask_previousAlign"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-11_noMask/";
	inF_names={
		"analysis_202211_20V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_40V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_60V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_80V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_100V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH8_-32to999.root",
		"analysis_202211_120V2ke_noMask_4Dabs100100_pl130None_minH7_2023-11_DUTalignMinH9_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "no_mask_newAlign"));

	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-11_noMask/";
	inF_names={
		"analysis_202211_20V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_40V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_60V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_80V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root",
		"analysis_202211_100V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH8_-32to999.root",
		"analysis_202211_120V2ke_noMask_4Dabs100100_pl24None_minH7_2023-11_DUTalignMinH9_-32to999.root"
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "no_mask_newAlign_pl24"));
	*/


	/*
	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-10/";
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_FEI4alignRel3.5_-32to999.root",
		"analysis_202211_20V2ke_f20_4Dabs100100_pl24None_minH7_2023-10_FEI4alignRel3.5_-32to999.root",
	};
	Xvec={19.5,21};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "20V_rel3.5"));//very slightly less
	*/

	/*
	canOpts.figFileName="2023-10_eff_furtherAlignment";
	canOpts.setValues(10, 130, 98.9, 100, "eff_vs_V", "V", "eff [%]", "ap", 900, 600);
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";

	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_60V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "FurtherAlign"));

	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_ChristopherAlignment_-32to999.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_ChristopherAlignment_-32to999.root",
		"analysis_202211_60V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_ChristopherAlignment_-32to999.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_ChristopherAlignment_-32to999.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-10_ChristopherAlignment_-32to999.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-10_ChristopherAlignment_-32to999.root",
	};
	Xvec={20,40,60,80,100,120};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "BeforeFurtherAlign"));


	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-8/";
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_60V2ke_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-1to999.root"
	};
	Xvec={20,40,60,100,120};
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "FurtherAlign_2023-8"));

	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",
		"analysis_202211_60V2ke_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",
		//"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",//not cut off
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root"
	};
	Xvec={20,40,60,100};
	YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "BeforeFurtherAlign_2023-8"));
	*/



	// plot 2023-8
	/*
	canOpts.inFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-8/";
	//canOpts.inFolder="/Users/ytian/code/output_tb/";
	canOpts.outFolder="./output_tb/";
	canOpts.setValues(10, 130, 98.9, 100, "eff_vs_V", "V", "eff [%]", "ap", 900, 600);
	canOpts.figFileName="2023-7_eff_comparison";
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";

	std::vector<TString> inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",
		"analysis_202211_60V2ke_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",
		//"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root",//not cut off
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root"
	};
	std::vector<double> Xvec={20,40,60,100};
	std::vector<std::vector<double>> YandUncs;
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "BeforeMoreAlign"));

	//pl130 further alignment (also cut away
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_60V2ke_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-1to999.root"
	};
	Xvec={20,40,60,100,120};
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "MoreAlign"));

	//FEI4
	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl24None_minH7_2023-7-29.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl24None_minH7_2023-7-29.root",
		"analysis_202211_60V2ke_4Dabs100100_pl24None_minH7_2023-7-29.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl24None_minH7_2023-7-29.root",
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl24None_minH7_2023-7-29.root",
		//"analysis_202211_120V2ke_f20_4Dabs100100_pl24None_minH7_2023-7-29_lv1.root"//lv1
	};
	Xvec={20,40,60,80,100};
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "Ref_FEI4"));

	inF_names={
		"analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1.root",
		"analysis_202211_40V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-32to999_run7818-7827.root",
		"analysis_202211_80V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29.root",
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1.root"
	};
	Xvec={20,40,80,120};
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "lv1_-32to999"));
	*/

	/*
	inF_names={
		"analysis_202211_100V2ke_f20_7913_7934_4Dabs100100_pl130None_minH7_2023-7-29_lv1.root",
	};
	Xvec={100};
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "lv1_-1to32"));

	inF_names={
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-1to400.root",
		//"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-32to1.root";
		//"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-32to400.root";
	};
	Xvec={120};
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "lv1_-1to400"));
	*/


	//------ ------ plot ------ ------
	//c1=initCan(canOpts, canOpts.TPads[0], canOpts.TPads[1]);//, "ALP RX");//causes grid exeed top
	c1=initCan(canOpts, canOpts.TPads[0], canOpts.TPads[1], "ap", "1");//, "ALP RX");//causes grid exeed top
	canOpts.TPads[0]->SetFillStyle(4000);
	canOpts.TPads[1]->SetFillStyle(4000);
	canOpts.legend=new TLegend(legendPos[0],legendPos[1],legendPos[2],legendPos[3]);
	for (int i=0;i<allTGasymms.size();i++){
		canOpts.legend->AddEntry(allTGasymms[i],allTGasymms[i]->GetTitle(),"p same");
		allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
		allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
		allTGasymms[i]->GetXaxis()->SetTitle(canOpts.Xaxis);
		allTGasymms[i]->GetYaxis()->SetTitle(canOpts.Yaxis);
		allTGasymms[i]->Draw("LP SAME");//RX reverses x axis
	}
	drawLegend(c1, canOpts.legend, canOpts.TPads[1]);
	//drawLegend(c1, canLeg, canOpts.TPads[0]);
	saveCan(c1, canOpts);

	/*
	std::vector<double> eyl;
	std::vector<double> eyh;

	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inFolder+"result_2022-10_2023-1_thres.txt");
	std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inFolder+"result_2022-10_thres_minH6.txt");

	
	i=1;
	std::vector<std::vector<double>> YandUncs=parser_eff_tb(effAndUnc);
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "min_hits_on_track=5"));
	canLeg->AddEntry(allTGasymms[i],"min_hits_on_track=5","p SAME");

	allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
	allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
	allTGasymms[i]->Draw("LP SAME");//RX reverses x axis
	*/

	//myChain->Print();
	//multiple of these, each to a file:
	//******************************************************************************
	//*Chain   :AnalysisITkPixQuad/plane110;1/eTotalEfficiency: /Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-8/analysis_202211_20V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_ChristopherAlignment.root *
	//******************************************************************************
	/*
	std::vector<TEfficiency*> allTEffs={};
	//("eff","different confidence levels;x;#epsilon",20,0,10);
	for(int i=0;i<1;i++){
		std::cout<<"1\n";
		p_TEff=(TEfficiency*)myChain.GetTree();
		std::cout<<"2\n";
		c1->cd();
		std::cout<<"3\n";
		//double 
		std::cout<<"4\n";
		std::cout<<"5\n";
		//p_TEff->Draw();
	}
	*/
	/*
		TEfficiency* p_TEff = (TEfficiency*)br_eff;
		//TEfficiency* p_TEff = (TEfficiency*)aTFile->Get(effPath);
		//TEfficiency* newTEff=new TEfficiency(*p_TEff);
		TH1* clone = p_TEff->GetCopyTotalHisto();
		TCanvas *c_clone = new TCanvas();
		clone->Draw();
		c_clone->Print("hist.png");
		//allTEffs.push_back(newTEff);
		//newTEff->Print();
		//aTFile->Close();
		p_TEff->Print();
	*/
	/*
	for(int i=0;i<inF_names.size();i++){
		allTEffs[i].Draw();
	}
	//putGraphsOnCanvas(c1, canOpts.legend, canOpts.TGEs, canOpts.TPads[1], 0, 2);
	saveCan(c1, canOpts);
	//theTGraphErrors->Draw();
	//printVector(effAndUnc,"effAndUnc: ");
	*/



	/*
	canOpts.setValues(0, 5500, 86, 100, "eff_vs_thres", "Threshold [e]", "Efficiency [%]", "ap", 900, 600);
	canOpts.figFileName="2023-1_eff_SCC4_vsThres";
	//auto c1 = new TCanvas("c1","A Simple Graph with asymmetric error bars",200,10,700,500);
	//c1->SetGrid();

	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inFolder+"result_2022-10_2023-1_thres.txt");
	std::vector<double> Xvec=getData_vector(allData_f1,1,0);
	std::vector<std::string> effAndUnc=get1row(allData_f1,4,0);
	std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inFolder+"result_2022-10_thres_minH6.txt");
	std::vector<double> Xvec2=getData_vector(allData_f1,1,0);
	std::vector<std::string> effAndUnc2=get1row(allData_f1,3,0);
	printVector(Xvec,"Xvec: ");
	printVector(Xvec2,"Xvec2: ");
	printVector(effAndUnc,"effAndUnc: ");
	printVector(effAndUnc2,"effAndUnc2: ");

	std::vector<TGraphAsymmErrors*> allTGasymms;
	int i=0;

	allTGasymms.push_back(makeTGAsymm(Xvec2, effAndUnc2, "SCC4"));
	canLeg->AddEntry(allTGasymms[i],"SCC4","p same");

	allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
	allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
	allTGasymms[i]->GetXaxis()->SetTitle("Threshold [e]");
	allTGasymms[i]->GetYaxis()->SetTitle("Efficiency [%]");
	allTGasymms[i]->Draw("LP SAME");//RX reverses x axis
	
	i=1;
	allTGasymms.push_back(makeTGAsymm(Xvec, effAndUnc, "min_hits_on_track=5"));
	canLeg->AddEntry(allTGasymms[i],"min_hits_on_track=5","p SAME");

	allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
	allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
	allTGasymms[i]->Draw("LP SAME");//RX reverses x axis
	*/




	/*
	canOpts.setValues(0.5, -50.5, 96, 99, "eff_vs_V", "Voltage [V]", "Efficiency [%]", "ap", 900, 600);
	canOpts.figFileName="2023-1_eff_SCC4_wMask97runs";
	c1=initCan(canOpts, canOpts.TPads[0], canOpts.TPads[1], "ALP RX");
	canOpts.TPads[0]->SetFillStyle(4000);
	canOpts.TPads[1]->SetFillStyle(4000);
	canOpts.TPads[1]->SetGrid(0,0);
	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inFolder+"result_2022-10.txt");
	std::vector<double> Xvec=getData_vector(allData_f1,1,0);
	std::vector<std::string> effAndUnc=get1row(allData_f1,2,0);
	//std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inFolder+"result_2022-10_alighWmask_analysisWmask.txt");
	//std::vector<double> Xvec2=getData_vector(allData_f2,1,0);
	//std::vector<std::string> effAndUnc2=get1row(allData_f2,2,0);
	std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inFolder+"result_2023-1_Nov2022_SCC4_mask97runs.txt");
	std::vector<double> Xvec2=getData_vector(allData_f2,1,0);
	std::vector<std::string> effAndUnc2=get1row(allData_f2,2,0);
	printVector(Xvec,"Xvec: ");
	printVector(Xvec2,"Xvec2: ");
	printVector(effAndUnc,"effAndUnc: ");
	printVector(effAndUnc2,"effAndUnc2: ");

	std::vector<TGraphAsymmErrors*> allTGasymms;
	int i=0;

	allTGasymms.push_back(makeTGAsymm(Xvec2, effAndUnc2, "SCC4"));
	canLeg->AddEntry(allTGasymms[i],"SCC4","p same");

	allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
	allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
	allTGasymms[i]->GetXaxis()->SetTitle("Voltage [V]");
	allTGasymms[i]->GetYaxis()->SetTitle("Efficiency [%]");
	allTGasymms[i]->Draw("LP RX SAME");//RX reverses x axis
	//aGraph=makeTGraphErrors(XYvec,gOptions)
	
	i=1;
	allTGasymms.push_back(makeTGAsymm(Xvec, effAndUnc, "SCC4_wo_mask"));
	canLeg->AddEntry(allTGasymms[i],"SCC4_wo_mask","p SAME");
	//allTGasymms.push_back(makeTGAsymm(Xvec2, effAndUnc2, "SCC4_with_mask"));
	//canLeg->AddEntry(allTGasymms[i],"SCC4_with_mask","lp SAME");

	allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
	allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
	allTGasymms[i]->Draw("LP RX SAME");//RX reverses x axis
	//aGraph=makeTGraphErrors(XYvec,gOptions)
	//putGraphsOnCanvas(c1,allTGasymms,canOpts.TPads[1]);
	*/




	/*
	canOpts.inFolder="/Users/ytian/hardware/2021_testBeam/tb_2023/";
	canOpts.outFolder="./output_tb/";

	canOpts.setValues(0.5, -120.5, 98, 100, "eff_vs_V", "Voltage [V]", "Efficiency [%]", "ap", 900, 600);
	canOpts.figFileName="2023-5_eff_Q9_refPl130";
	//canOpts.figFileName="2023-5_eff_Q9";
	c1=initCan(canOpts, canOpts.TPads[0], canOpts.TPads[1], "ALP RX");
	canOpts.TPads[0]->SetFillStyle(4000);
	canOpts.TPads[1]->SetFillStyle(4000);
	canOpts.TPads[1]->SetGrid(0,0);
	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inFolder+"result_2022-11.txt");
	std::vector<double> Xvec=getData_vector(allData_f1,1,0);
	std::vector<std::string> effAndUnc=get1row(allData_f1,3,0);
	printVector(Xvec,"Xvec: ");
	printVector(effAndUnc,"effAndUnc: ");

	std::vector<TGraphAsymmErrors*> allTGasymms;
	int i=0;

	allTGasymms.push_back(makeTGAsymm(Xvec, effAndUnc, "Q9"));
	canLeg->AddEntry(allTGasymms[i],"Q9","p same");

	allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
	allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
	allTGasymms[i]->GetXaxis()->SetTitle("Voltage [V]");
	allTGasymms[i]->GetYaxis()->SetTitle("Efficiency [%]");
	allTGasymms[i]->Draw("LP RX SAME");//RX reverses x axis
	//aGraph=makeTGraphErrors(XYvec,gOptions)
	
	drawLegend(c1, canLeg, canOpts.TPads[0]);
	//c1->Print("./2023-1_eff_SCC4_vsThres.pdf");
	//c1->Print(canOpts.figFileName+".pdf");
	saveCan(c1, canOpts);
	*/
}
