//
// Created by Thibault PLET on 31/05/2016.
//

#ifndef COM_OSTERES_AUTOMATION_ARDUINO_COMPONENT_DATABUFFER_H
#define COM_OSTERES_AUTOMATION_ARDUINO_COMPONENT_DATABUFFER_H

class DataBuffer {
public:
    /**
     * Constructor
     *
     * @param delay      Delay for buffer validity, in milliseconds
     * @param firstDelay Delay for first outdate, in milliseconds. Immediate by default.
     */
    DataBuffer(unsigned int delay = 10000, unsigned int firstDelay = 0)
    {
        // Init
        this->bufferDelay = delay; // ms
        this->bufferTimePoint = millis() - delay + firstDelay;
    }

    /**
     * Destructor
     */
    ~DataBuffer()
    {
        // Nothing
    }

    /**
     * Reset buffer
     */
    virtual void reset()
    {
        this->resetBufferTimePoint();
    }

    /**
     * Flag to indicate if buffer is considering as outdated
     */
    bool isOutdated()
    {
        return millis() - this->bufferTimePoint > this->getBufferDelay();
    }

    /**
     * Get time remain before outdate (in ms). Remain countdown stop to 0.
     */
    unsigned long getRemain()
    {
        long v = this->getBufferDelay() - (millis() - this->bufferTimePoint);
        return v < 0 ? 0 : v;
    }

    /**
     * Set buffer as outdated imediately
     */
    void setOutdated()
    {
        this->bufferTimePoint = millis() - this->getBufferDelay();
    }

    /**
     * Get limit delay at which buffer is considering as outdated (in milliseconds)
     */
    unsigned long getBufferDelay()
    {
        return this->bufferDelay;
    }

    /**
     * Set limit delay at which buffer is considered as outdated (in milliseconds)
     */
    void setBufferDelay(unsigned int value)
    {
        this->bufferDelay = value;
    }
protected:
    /**
     * Reset buffer time point
     */
    void resetBufferTimePoint()
    {
        this->bufferTimePoint = millis();
    }

    /**
     * Limit interval before buffer is considering as outdated (in milliseconds)
     */
    unsigned int bufferDelay;

    /**
     * Time point for buffer
     */
    unsigned long bufferTimePoint;
};

#endif //COM_OSTERES_AUTOMATION_ARDUINO_COMPONENT_DATABUFFER_H
