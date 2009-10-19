/*
**    THE ARMADEUS PROJECT
** 
**    Copyright (C) 2009  The armadeus systems team 
**    Fabien Marteau <fabien.marteau@armadeus.com>
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "as_max1027.hpp"


const char * AsMax1027::SETUP_NAME = "setup";                 
const char * AsMax1027::CONVERSION_NAME = "conversion";       
const char * AsMax1027::TEMPERATURE_NAME = "temp1_input";     
const char * AsMax1027::INPUT_PRE_NAME = "in";  
const char * AsMax1027::INPUT_POST_NAME = "_input";  
const char * AsMax1027::SYS_PATH = "/sys/bus/spi/devices/spi";

AsMax1027 * AsMax1027::mMax1027 = NULL;

/** Singleton constructor
 *
 * @return class pointer
 */
AsMax1027*
AsMax1027::getInstance(int aSpiNum)
{
    if (mMax1027 == NULL)
    {
        mMax1027 = new AsMax1027(aSpiNum);
        mMax1027->init(aSpiNum);
    }
    return mMax1027;
}

/*------------------------------------------------------------------------------*/
 
AsMax1027::AsMax1027(int aSpiNum):AsAdc(CHANNELS_NUM)
{
    int iterator;

    mSpiNumber = aSpiNum;
    /* init files values */
    mFSetup = -1;
    mFTemp = -1;
    mFConversion = -1;
    for (iterator = 0; iterator< CHANNELS_NUM; iterator++)
    {
        setChannelHandler(-1, iterator);
    }

}

/*------------------------------------------------------------------------------*/
 
AsMax1027::~AsMax1027()
{
    int file;
    int iterator;

    /* close setup */
    if (mFSetup >= 0) {
        close(mFSetup); 
    }
    
    /* close Temperature file */
    if (mFTemp >= 0) {
        close(mFTemp);
    }

    /* close conversion */
    if (mFConversion >= 0) {
        close(mFConversion);
    }

    /* close channels */
    for (iterator= 0; iterator < CHANNELS_NUM; iterator++)
    {
        file = getChannelHandler(iterator);
        if (file >= 0)
        {
            close(file);
        }
    }
    mMax1027 = NULL;
}
/*------------------------------------------------------------------------------*/

/** Initialisation of adc
 *
 * @param aSpiNumber: the number of the spi bus used
 * @return error
 */
AsAdc::AsAdc_errors
AsMax1027::init(int aSpiNumber)
{
    char spiNumString[2];
    string str_tmp;

    /* set the /sys path */
    setSpiNumber(aSpiNumber);
    snprintf(spiNumString, 2, "%d", getSpiNumber());
    str_tmp = string(SYS_PATH);
    str_tmp = str_tmp + string(spiNumString);
    str_tmp = str_tmp + string(".0/");
    setSysPath(str_tmp);
    
    /* open setup */
    mFSetup = open(string(getSysPath()+string(SETUP_NAME)).c_str(),O_RDWR);
    if (mFSetup < 0)
    {
        perror(string(
                string("Can't open setup file. Is driver max1027 modprobed ?\n")+\
                getSysPath()+string(SETUP_NAME)+string(" ")).c_str());
        return AS_ADC_OPENFILE; 
    }

    /* open temperature */
    mFTemp = open(string(getSysPath()+string(TEMPERATURE_NAME)).c_str(), O_RDONLY);
    if (mFTemp < 0)
    {
        perror(string(
                string("Can't open temperature file. Is driver max1027 modprobed ?\n")+\
                getSysPath()+string(TEMPERATURE_NAME)+string(" ")).c_str());
        return AS_ADC_OPENFILE; 
    }

    /* open conversion */
    mFConversion = open(string(getSysPath()+string(CONVERSION_NAME)).c_str(),O_WRONLY);
    if (mFConversion < 0)
    {
        perror(string(
                string("Can't open setup file. Is driver max1027 modprobed ?\n")+\
                getSysPath()+string(CONVERSION_NAME)+string(" ")).c_str());
        return AS_ADC_OPENFILE; 
    }

    /* open each channel */
    {
        int channel_num;
        char channel_string_num[2];
        int fChannel;

        for (channel_num=0; channel_num < CHANNELS_NUM; channel_num++)
        {
            snprintf(channel_string_num,2, "%1d", channel_num);
            fChannel = open(string(getSysPath()+\
                                   string(INPUT_PRE_NAME)+\
                                   string(channel_string_num)+\
                                   string(INPUT_POST_NAME)
                                   ).c_str(),O_RDONLY);
            if (fChannel < 0)
            {
                perror(string(
                              string("Can't open channel file. Is /dev files created ?\n")+\
                              string(INPUT_PRE_NAME)+\
                              string(channel_string_num)).c_str());
                return AS_ADC_OPENFILE;
            }
            setChannelHandler(fChannel, channel_num);
        }
    }

    return AS_ADC_OK;
}

/*------------------------------------------------------------------------------*/

/** write a value in ASCII in file given in argument
 *
 * @param aValue: int value
 * @return the string value
 */
string
AsMax1027::setIntValueInString(int aValue)
{
    char valueString[10];

    snprintf(valueString, 10, "%d", aValue);
    return string(valueString);
}

/*------------------------------------------------------------------------------*/

/** Read temperature of max1027 chip
 *
 * @return temperature value in m°C, if value negative return error
 */
AsAdc::AsAdc_errors
AsMax1027::readTemperature_mC(void)
{
    string valueWrite;
    string valueRead;
    int ret;

    /* configure setup */
    valueWrite = setIntValueInString(0x62);
    ret = writeBuffer(mFSetup, valueWrite);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }

    /* start conversion */
    valueWrite = setIntValueInString(0x87);
    ret = writeBuffer(mFConversion, valueWrite);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }
 
    /* read value */
    ret = readBuffer(mFTemp, &valueRead);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }
    return (AsAdc_errors)atoi(valueRead.c_str());
}

/*------------------------------------------------------------------------------*/

/** Read temperature of max1027 chip
 *
 * @return temperature value in °C
 */
float
AsMax1027::readTemperature_C(void)
{
    return (((float)readTemperature_mC())/1000.0f);
}

/*------------------------------------------------------------------------------*/

/** Read analog value in mV
 *
 * @param aObject: channel number
 * @return voltage value in mV, if value negative return error
 */
AsAdc::AsAdc_errors
AsMax1027::getValueInMiliVolt(void * aObject)
{
    int fAdc;
    int ret;
    int channelNum;
    string valueRead;
    string valueWrite;
    
    channelNum = getObjectChannel(aObject);
    if (channelNum < 0)
    {
        return AS_ADC_WRONGCHAN;
    }

    fAdc = getChannelHandler(channelNum);
    /* configure setup */
    valueWrite = setIntValueInString(0x62);
    ret = writeBuffer(mFSetup, valueWrite);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }

    /* start conversion */
    valueWrite = setIntValueInString(0x86 | (channelNum << 3));
    ret = writeBuffer(mFConversion, valueWrite);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }

    /* read value */
    ret = readBuffer(fAdc, &valueRead);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }
    return (AsAdc_errors)atoi(valueRead.c_str());
}

/*------------------------------------------------------------------------------*/

/** Read analog value in mV
 *
 * @param aChannelNumber channel number
 * @return voltage value in mV, if value negative return error
 */
AsAdc::AsAdc_errors
AsMax1027::getValueInMiliVolt(int aChannelNumber)
{
    int fAdc;
    int ret;
    string valueRead;
    string valueWrite;

    if (mChannelNumber < aChannelNumber)
    {
        return AS_ADC_WRONGCHAN;
    }

    fAdc = getChannelHandler(aChannelNumber);
    if(fAdc < 0)
    {
        return AS_ADC_READFILE;
    }

    /* configure setup */
    valueWrite = setIntValueInString(0x62);
    ret = writeBuffer(mFSetup, valueWrite);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }

    /* start conversion */
    valueWrite = setIntValueInString(0x86 | (aChannelNumber << 3));
    ret = writeBuffer(mFConversion, valueWrite);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }

    /* read value */
    ret = readBuffer(fAdc, &valueRead);
    if (ret < 0)
    {
        return (AsAdc_errors)ret;
    }
    return (AsAdc_errors)atoi(valueRead.c_str());
}


/*------------------------------------------------------------------------------*/

/** Read analog value in Volt
 *
 * @param aObject: channel number
 * @return voltage value in V
 */
float
AsMax1027::getValueInVolt(void * aObject)
{
    int ret;

    ret = getValueInMiliVolt(aObject);
    if (ret < 0)
    {
        return (float)ret;
    }
    return (((float)ret)/1000.0f);
}
/*------------------------------------------------------------------------------*/

/** Read analog value in Volt
 *
 * @param aObject: channel number
 * @return voltage value in V
 */
float
AsMax1027::getValueInVolt(int aChannelNumber)
{
    int ret;

    ret = getValueInMiliVolt(aChannelNumber);
    if (ret < 0)
    {
        return (float)ret;
    }
    return (((float)ret)/1000.0f);
}
/*------------------------------------------------------------------------------*/

/** Get file handler for analog channel
 *
 * @param aChannel_num: channel number
 * @return File handler (int)
 */
AsAdc::AsAdc_errors
AsMax1027::getChannelHandler(int aChannel_num)
{
    if ((aChannel_num < 0) || (aChannel_num > CHANNELS_NUM))
    {
        return AS_ADC_WRONGCHAN;
    }
    return (AsAdc_errors)mChannelHandlers[aChannel_num];
}

/*------------------------------------------------------------------------------*/

/** Set file handler for analog channel
 * 
 * @param aFchannel: File handler
 * @param aChannel_num: channel number
 * @return File handler (int)
 */
AsAdc::AsAdc_errors
AsMax1027::setChannelHandler(int aFchannel,int aChannel_num)
{
    if (aChannel_num < 0 || aChannel_num > CHANNELS_NUM)
    {
        return AS_ADC_WRONGCHAN;
    }
    mChannelHandlers[aChannel_num] = aFchannel;
    return AS_ADC_OK;
}

/*------------------------------------------------------------------------------*/

/** Read a string value in file
 *
 * @param aFile_handler: file handler
 * @param aValueRead: string ptr
 * @return error code
 */
AsAdc::AsAdc_errors
AsMax1027::readBuffer(int aFile_handler, string *aValueRead)
{
    const int SIZEOFBUFF = 20;
    ssize_t ret;
    char valueRead[SIZEOFBUFF];

    ret = read(aFile_handler, valueRead, SIZEOFBUFF);
    if (ret < 0)
    {
        perror("Read error ");
        printf("error %d\n",ret);
        return AS_ADC_READFILE;
    }
    valueRead[ret] = '\0';
    ret = lseek(aFile_handler, 0, SEEK_SET);
    if (ret < 0)
    {
        perror("lseek error ");
        return AS_ADC_LSEEK;
    }
    *aValueRead = string(valueRead);
    return AS_ADC_OK;
}

/*------------------------------------------------------------------------------*/

/** Write a string value in file
 *
 * @param aFile_handler: file handler
 * @param aValueWrite: string value
 * @return error code
 */
AsAdc::AsAdc_errors
AsMax1027::writeBuffer(int aFile_handler, string aValueWrite)
{
    int ret;
    ret = write(aFile_handler, aValueWrite.c_str(), aValueWrite.length());
    if (ret < 0)
    {
        perror("Can't write file ");
        return AS_ADC_WRITEFILE;
    }
    ret = lseek(aFile_handler,0,SEEK_SET);
    if (ret < 0)
    {
        perror("lseek error ");
        return AS_ADC_LSEEK;
    }
    return AS_ADC_OK;
}
