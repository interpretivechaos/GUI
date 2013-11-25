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

#ifndef __MATLABENGINE_H_5DD33DB4__
#define __MATLABENGINE_H_5DD33DB4__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../AccessClass.h"
#include "../UI/DataViewport.h"

#include <engine.h>
#include <matrix.h>

class MatlabTerminal;

class MatlabEngineInterface : public AccessClass
{
public: 
/** Constructor */
    MatlabEngineInterface();

/** Destructor */
    ~MatlabEngineInterface();

bool initializeMatlabEngine();

void sendBufferToMatlab(AudioSampleBuffer& buffer, String bufferVarName, MidiBuffer& eventBuffer, int& nSamples, int nodeID=-1);

void disconnectBufferToMatlab(AudioSampleBuffer& buffer, MidiBuffer& eventBuffer, int& nSamples, int nodeID);

void sendCommandToMatlab(String command);

String getMatlabOut();

void sendTerminalCommandToMatlab();



private:

    int numNodesToMatlab;
    int numNodesFromMatlab;
    int numNodes;
    mxArray* toMatlab;
    mxArray* fromMatlab;
    Engine* ep;
    bool engineInitialized;
    ScopedPointer<MatlabTerminal> matlabTerminal;

    const int engOutBufferSize=256;
    char engOutBuffer[257];
    String MatlabOutputBuffer;
};


class MatlabTerminal : public AccessClass, public Component

{
    public:
    MatlabTerminal();
    ~MatlabTerminal();
    void setMatlabEngineInterface(MatlabEngineInterface* _me)
    {
         me=_me;
    }

    String getTerminalInput();

    void resetTerminal();

    /** Draws the MatlabTerminal.*/
    void paint(Graphics& g);

private:

    /** The text displayed to the user.*/
    String infoString;

    /** Font used to draw the label.*/
    Font labelFont;

    /** Label: sends commands to matlab Engine */
    ScopedPointer<Label> terminalInput;

    /** Pointer to Matlab Engine */
    MatlabEngineInterface* me;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatlabTerminal);

};





#endif  // __MATLABENGINE_H_5DD33DB4__
