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
    // numChannels = 1;

    const char* fieldNames[] = {"nodeID", "Type", "Data"};
    // Creates a numChannels column vector of structure, with field types specified above 

    toMatlab = mxCreateStructMatrix(numNodesToMatlab, 1, sizeof(fieldNames), fieldNames);
    fromMatlab = mxCreateStructMatrix(numNodesToMatlab, 1, sizeof(fieldNames), fieldNames);
}

MatlabEngineInterface::~MatlabEngineInterface()
{
    
}

bool MatlabEngineInterface::InitializeMatlabEngine(){
	// Prevent different threads from using any Engine functions
	std::lock_guard<std::recursive_mutex> lock(mutex);
	
    if (engineInitialized == FALSE)
    {
        if (!(ep = engOpen(""))) {
            std::cout<<"Can't start MATLAB engine"<<std::endl;
            return FALSE;
        } else {
            return TRUE;
        }
    } else {
        std::cout << "Matlab Engine Already Initialized"<<std::endl;
        return FALSE;
    }
}

void MatlabEngineInterface::SendBufferToMatlab(AudioSampleBuffer& buffer, MidiBuffer& eventBuffer, int& nSamples, int nodeID=-1){
	// Prevent different threads from using any Engine functions
	std::lock_guard<std::recursive_mutex> lock(mutex);

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    numNodesToMatlab++;

    bool makeNewNode = TRUE;
    int nodeIndex = numNodesToMatlab;
    if (nodeID==-1)
	{
		nodeID=numNodesToMatlab;
	}
    for (int i = 0; i < numNodesToMatlab; ++i)
    {
        
        mxArray * nodeField=mxGetField(toMatlab, i, "nodeID");
        int extractedNodeID=mxGetScalar(nodeField);
        if (nodeID == extractedNodeID)
         {
             std::cout<< "nodeID already exists";
             makeNewNode = FALSE;
             nodeIndex = i;
         }
    }

    //Sets number of rows in toMatlab to number of nodes sending information.
    if (makeNewNode)
    {
        mxSetM(toMatlab, numNodesToMatlab);
    }


    // mxArray* toMatlab = mxCreateStructMatrix(numChannels, numNodesToMatlab, sizeof(fieldNames), fieldNames);

    // remember to add midibuffer support
    // Create Array for each channel
    const char* dataFieldNames[] = {"ChannelName", "BufferContents"};
    mxArray* bufferContents = mxCreateStructMatrix(numChannels, numSamples, sizeof(dataFieldNames), dataFieldNames);
    float * fs;
    double * ds;
    mxArray * sample = mxCreateDoubleScalar(0);

    for (int i = 0; i < numChannels; ++i)
    {
        // Creates double vector for each channel and copies it over
        

        for (int j =0; j < numSamples; ++j)
        {
            // Convert
            fs = buffer.getSampleData(i, j);
            *ds=*fs;
            mxSetPr(sample, ds);
            mxSetField(bufferContents,(i*numSamples+j), "BufferContents", sample);

        }
        mxSetField(toMatlab, i, "Data", bufferContents);
    }

//    mxSetField(toMatlab, nodeIndex, "BufferContents", mxArray *pvalue);
//
//    *mxCreateStructMatrix(numNodes, 1, int nfields, "nodeID");
    numNodes++;
}


void MatlabEngineInterface::disconnectBufferToMatlab(AudioSampleBuffer& buffer, MidiBuffer& eventBuffer, int& nSamples, int nodeID){
    //
    numNodes--;
}
