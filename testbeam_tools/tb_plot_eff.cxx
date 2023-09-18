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
		TString fullPath=canOpts.inputFolder+inF_names[i].Data();
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
	canOpts.inputFolder="/Users/ytian/hardware/2021_testBeam/analysis/";
	canOpts.outputFolder="./output_tb/";
	std::vector<std::string> fileNameVec={};
	std::vector<std::string> legNameVec={};
	TCanvas* c1; TPad* pad0; TPad* pad1; canOpts.TPads={pad0,pad1};
	std::vector<std::vector<std::vector<std::string>>> allData;
	std::vector<std::vector<std::vector<double>>> allXYvecs;
	std::vector<int> indices;

	canOpts.factorsX={1.}; canOpts.factorsY={1.};
	canOpts.indexRow=0;// index of the row below which data starts
	canOpts.indexColX=1;
	canOpts.indexColY=2;
	//canOpts.indexColXunc=-1;
	//canOpts.indexColYunc=-1;
	canOpts.plotType="eff";
	canOpts.markerSizes[0]=1.25; canOpts.markerSizes.push_back(1.25);
	canOpts.marginT=0.1; canOpts.marginB=0.1; canOpts.marginL=0.13; canOpts.marginR=0.07; canOpts.xLabelSize=0.035;

	TLegend* canLeg=new TLegend(0.72,0.05,1.0,0.95);
	c1=initCan(canOpts, canOpts.TPads[0], canOpts.TPads[1]);
	canOpts.TPads[0]->SetFillStyle(4000);
	canOpts.TPads[1]->SetFillStyle(4000);

	
	//canOpts.inputFolder="/Users/ytian/code/output_tb/";
	canOpts.inputFolder="/Users/ytian/hardware/tb/2022-11_analysis_2023-7/analysis_2023-8/";
	canOpts.outputFolder="./output_tb/";
	canOpts.setValues(10, 130, 98.9, 100, "eff_vs_V", "V", "eff [%]", "ap", 900, 600);
	canOpts.figFileName="2023-7_eff_comparison";
	c1=initCan(canOpts, canOpts.TPads[0], canOpts.TPads[1]);//, "ALP RX");//causes grid exeed top
	TGE theOptions;//theOptions.colour=1;theOptions.markerSize=1;theOptions.lineWidth=1
	TString folderInRoot="AnalysisITkPixQuad/plane110/eTotalEfficiency";
	std::vector<TGraphAsymmErrors*> allTGasymms;

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
	};
	Xvec={120};
	YandUncs=tb_getYandUncs(inF_names, canOpts, folderInRoot);
	printVectorVector(YandUncs,"YandUncs: ");
	printVector(Xvec,"Xvec: ");
	allTGasymms.push_back(makeTGAsymm(Xvec, YandUncs, "lv1_-1to400"));
	*/
	std::vector<TString> legTexts={
		"BeforeMoreAlign",
		"MoreAlign",
		"Ref_FEI4",
		"lv1_-32to999",
		//"lv1_-1to32",
		//"lv1_-1to400",
	};
	/*
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-32to1.root";
		"analysis_202211_120V2ke_f20_4Dabs100100_pl130None_minH7_2023-7-29_lv1_-32to400.root";
	*/
	for (int i=0;i<allTGasymms.size();i++){
		canLeg->AddEntry(allTGasymms[i],legTexts[i],"p same");
		allTGasymms[i]->SetMarkerColor(canOpts.colours[i]);
		allTGasymms[i]->SetMarkerStyle(canOpts.markers[i]);
		allTGasymms[i]->GetXaxis()->SetTitle("Threshold [e]");
		allTGasymms[i]->GetYaxis()->SetTitle("Efficiency [%]");
		allTGasymms[i]->Draw("LP SAME");//RX reverses x axis
	}
	drawLegend(c1, canLeg, canOpts.TPads[0]);
	//c1->Print("./2023-1_eff_SCC4_vsThres.pdf");
	//c1->Print(canOpts.figFileName+".pdf");
	saveCan(c1, canOpts);

	/*
	std::vector<double> eyl;
	std::vector<double> eyh;

	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inputFolder+"result_2022-10_2023-1_thres.txt");
	std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inputFolder+"result_2022-10_thres_minH6.txt");

	
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

	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inputFolder+"result_2022-10_2023-1_thres.txt");
	std::vector<double> Xvec=getData_vector(allData_f1,1,0);
	std::vector<std::string> effAndUnc=get1row(allData_f1,4,0);
	std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inputFolder+"result_2022-10_thres_minH6.txt");
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
	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inputFolder+"result_2022-10.txt");
	std::vector<double> Xvec=getData_vector(allData_f1,1,0);
	std::vector<std::string> effAndUnc=get1row(allData_f1,2,0);
	//std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inputFolder+"result_2022-10_alighWmask_analysisWmask.txt");
	//std::vector<double> Xvec2=getData_vector(allData_f2,1,0);
	//std::vector<std::string> effAndUnc2=get1row(allData_f2,2,0);
	std::vector<std::vector<std::string>> allData_f2=getData(canOpts.inputFolder+"result_2023-1_Nov2022_SCC4_mask97runs.txt");
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
	canOpts.inputFolder="/Users/ytian/hardware/2021_testBeam/tb_2023/";
	canOpts.outputFolder="./output_tb/";

	canOpts.setValues(0.5, -120.5, 98, 100, "eff_vs_V", "Voltage [V]", "Efficiency [%]", "ap", 900, 600);
	canOpts.figFileName="2023-5_eff_Q9_refPl130";
	//canOpts.figFileName="2023-5_eff_Q9";
	c1=initCan(canOpts, canOpts.TPads[0], canOpts.TPads[1], "ALP RX");
	canOpts.TPads[0]->SetFillStyle(4000);
	canOpts.TPads[1]->SetFillStyle(4000);
	canOpts.TPads[1]->SetGrid(0,0);
	std::vector<std::vector<std::string>> allData_f1=getData(canOpts.inputFolder+"result_2022-11.txt");
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
