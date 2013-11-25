/*
  ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "MatlabEngine.h"

MatlabEngineInterface::MatlabEngineInterface() {
    // specifies number of processors sending information to matlab
    numNodesToMatlab = 0;
    numNodesFromMatlab = 0;
    numNodes=0;
    engineInitialized=FALSE;

    // numChannels = 1;

    const char* fieldNames[] = {"nodeID", "Type", "Data"};
    // Creates a numChannels column vector of structure, with field types specified above 
    int numberOfFields=sizeof(fieldNames)/sizeof(*fieldNames);
    
    toMatlab = mxCreateStructMatrix(1, 1, numberOfFields, fieldNames);
    mxArray* nodeField=mxCreateDoubleScalar(1);
    mxSetField(toMatlab, 1, "nodeID", nodeField);
    // fromMatlab = mxCreateStructMatrix(1, 1, numberOfFields, fieldNames);
    // mxSetField(fromMatlab, 1, "nodeID", nodeField);

    // std::cout << "Created MatlabEngineInterface with address "<< this << std::endl;
}

MatlabEngineInterface::~MatlabEngineInterface()
{
    
}

bool MatlabEngineInterface::initializeMatlabEngine(){


    if (!engineInitialized)
    {
        std::cout<<"initializeMatlabEngine: engineInitialized ==" << engineInitialized <<"@" << this << std::endl;

        // Prevent different threads from using any Engine functions
        std::lock_guard<std::recursive_mutex> lock(std::mutex);

        if (!(ep = engOpen(""))) {
            sendActionMessage("Can't start MATLAB engine");
            // std::cout<<"Can't start MATLAB engine"<<std::endl;
            return FALSE;
        }
        else
        {

            // std::cout<<"Started MATLAB engine"<<std::endl;
            sendActionMessage("Started MATLAB engine");
            engineInitialized =TRUE;
            engOutBuffer[engOutBufferSize]='\0';
            engOutputBuffer(ep, engOutBuffer, engOutBufferSize);

            matlabTerminal = new MatlabTerminal();
            matlabTerminal->setMatlabEngineInterface(this);
            getDataViewport()->addTabToDataViewport("MATLAB", matlabTerminal, 0);
            std::cout<<"Added MATLAB Terminal Tab"<<std::endl;
            return TRUE;

        }
    }
    else {
        std::cout << "Matlab Engine Already Initialized"<<std::endl;
        sendActionMessage("Matlab Engine Already Initialized");

        return FALSE;
    }
}

void MatlabEngineInterface::sendBufferToMatlab(AudioSampleBuffer& buffer, String bufferVarName, MidiBuffer& eventBuffer, int& nSamples, int nodeID){

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();

 //    bool makeNewNode = TRUE;
 //    int nodeIndex = numNodesToMatlab;
 //    if (nodeID > numNodesToMatlab)
 //    {
 //        makeNewNode = TRUE;
 //    }
 //    else if (nodeID==-1)
	// {
	// 	nodeID=numNodesToMatlab;
	// }
 //    else{
 //    for (int i = 0; i < numNodesToMatlab; ++i)
 //    {
 //            std::cout<< "nodeID" << nodeID <<std::endl;
        
 //            //BUG:: EXC BAD ACCESS here, likely because nodeField isn't being made properly.
 //            mxArray * nodeField=mxGetField(toMatlab, i, "nodeID");
 //            // mxArray * nodeField = mxGetFieldByNumber(toMatlab, i, 1);
 //            std::cout<< "nodeField@" << nodeField <<std::endl;
 //            int extractedNodeID=mxGetScalar(nodeField);
 //            if (nodeID == extractedNodeID)
 //            {
 //                std::cout<< "nodeID already exists" <<std::endl;
 //                makeNewNode = FALSE;
 //                nodeIndex = i;
 //            }
 //        }
 //    }

 //    //Sets number of rows in toMatlab to number of nodes sending information.
 //    if (makeNewNode)
 //    {
 //        mxSetM(toMatlab, numNodesToMatlab);
 //    }


    // mxArray* toMatlab = mxCreateStructMatrix(numChannels, numNodesToMatlab, sizeof(fieldNames), fieldNames);

    // remember to add midibuffer support
    // Create Array for each channel
    const char* dataFieldNames[] = {"ChannelName", "BufferContents"};
        
    int numberOfFields=sizeof(dataFieldNames)/sizeof(*dataFieldNames);
    mxArray* bufferContents = mxCreateStructMatrix(numChannels, numSamples, numberOfFields, dataFieldNames);
    double * ds;
    ds = (double*)mxMalloc(sizeof(double));
    mxArray * sample = mxCreateDoubleScalar(0);

    for (int i = 0; i < numChannels; ++i)
    {        
        // doubleArray
        for (int j =0; j < numSamples; ++j)
        {
            // Convert
            *ds=*(double*)buffer.getSampleData(i,j);
            if (!ds) {
                ds=0;
            }
            std::cout<<*ds<<"j:"<<j<<" i:"<< i<< std::endl;
            mxSetPr(sample, ds);
            engPutVariable(ep, "ds", sample);

            mxSetField(bufferContents,(i*numSamples+j), "BufferContents", sample);

        }
    }
    
    if (engineInitialized) {
        // std::cout<<"engPutVariable "<<std::endl;
        engPutVariable(ep, "fromGUI", bufferContents);
        // engPutVariable(ep, bufferVarName.toUTF8(), bufferContents);


//        engPutVariable(ep, bufferVarName.toUTF8(), bufferContents);
    }
//    mxSetField(toMatlab, nodeIndex, "BufferContents", mxArray *pvalue);
//
//    *mxCreateStructMatrix(numNodes, 1, int nfields, "nodeID");
    // numNodes++;
}


void MatlabEngineInterface::disconnectBufferToMatlab(AudioSampleBuffer& buffer, MidiBuffer& eventBuffer, int& nSamples, int nodeID){
    //
    numNodes--;
}

void MatlabEngineInterface::sendCommandToMatlab(String command){
    engEvalString(ep, command.toUTF8());
}

String MatlabEngineInterface::getMatlabOut(){
    String MatlabOutputBuffer(engOutBuffer, engOutBufferSize);
    std::cout<<"engOutputBuffer: " << engOutBuffer <<std::endl;
    std::cout<<"MatlabOutputBuffer: "<< MatlabOutputBuffer <<std::endl;

    return MatlabOutputBuffer;
}

void MatlabEngineInterface::sendTerminalCommandToMatlab(){
    String command=matlabTerminal->getTerminalInput();
//    std::cout<< command.getCharPointer() <<std::endl;
    engEvalString(ep, command.toUTF8());
    matlabTerminal->resetTerminal();
}


MatlabTerminal::MatlabTerminal()
{

    labelFont = Font("Paragraph", 24, Font::plain);

    // MemoryInputStream mis(BinaryData::misoserialized,
    //                       BinaryData::misoserializedSize,
    //                       false);
    // Typeface::Ptr tp = new CustomTypeface(mis);
    // labelFont = Font(tp);
    // labelFont.setHeight(24);

    infoString = "This is the Matlab Engine Console. This allows commands to be sent to the engine.";
    terminalInput = new Label("terminalInput", "Enter MATLAB command here");
    terminalInput->setBounds("10, parent.height - 34, parent.width -10, top + 24");
    terminalInput->setEditable(true);
    terminalInput->showEditor();
    TextEditor* te=terminalInput->getCurrentTextEditor();
    te->grabKeyboardFocus();

    addAndMakeVisible(terminalInput);

}

MatlabTerminal::~MatlabTerminal()
{

}

String MatlabTerminal::getTerminalInput(){
    return terminalInput->getText(true);

}

void MatlabTerminal::resetTerminal(){
    terminalInput->showEditor();
    TextEditor* te=terminalInput->getCurrentTextEditor();
    te->grabKeyboardFocus();
    infoString=me->getMatlabOut();
    repaint();
}

void MatlabTerminal::paint(Graphics& g)
{
    g.fillAll(Colours::grey);

    g.setFont(labelFont);

    g.setColour(Colours::black);

    g.drawFittedText(infoString, 10, 10, getWidth()-10, getHeight()-10, Justification::left, 100);

}
