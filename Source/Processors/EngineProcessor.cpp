
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


#include <stdio.h>
#include "EngineProcessor.h"

EngineProcessor::EngineProcessor()
    : GenericProcessor("Send to Matlab") //, threshold(200.0), state(true)

{
    String defaultName("node");
    defaultName=defaultName+String(getNodeId());
    parameters.add(Parameter("Variable Name", defaultName, 1));
}

EngineProcessor::~EngineProcessor()
{

}



void EngineProcessor::setParameter(int parameterIndex, float newValue)
{
    editor->updateParameterButtons(parameterIndex);

    //Parameter& p =  parameters.getReference(parameterIndex);
    //p.setValue(newValue, 0);

    //threshold = newValue;

    //std::cout << float(p[0]) << std::endl;
    editor->updateParameterButtons(parameterIndex);
}

bool EngineProcessor::enable()
{
    matlabEngine=getMatlabEngineInterface();

    matlabEngine->initializeMatlabEngine();

    return isEnabled;
}

void EngineProcessor::process(AudioSampleBuffer& buffer,
                               MidiBuffer& events,
                               int& nSamples)
{

    // for (int i = 0; i < nSamples; i++)
    // {
    //     // std::cout << "Send Buffer TO Matlab @" << matlabEngine<< std::endl;

        matlabEngine->sendBufferToMatlab(buffer, getParameterVar(0,1), events, nSamples, -1);
    // }


}
