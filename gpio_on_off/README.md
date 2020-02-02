Uses GPIO and callbacks to respond to button state changes. Tested on BBC micro:bit.

Tested on BBC microbit.

Build with the Zephyr source v1.14-branch

What is GPIO?
-------------
It stands for General Purpose Input Output and is perhaps the most basic of I/O interfaces a computer or microntroller has. It has two operating modes, input and output. Input lets you read the voltage on a pin, which will either be low (0v) or high (3.3v on a microbit). Output lets you set the voltage on a pin to either high or low. In this way, the state of things connected to pins can be determined and responded to in code or their state can be changed. In other words, GPIO lets you both monitor and control, simple, connected electronic devices.

For example, with an LED and suitable resistor connected in series between pin 0 and the GND pin of a microbit, using its edge connector, setting the state of pin 0 high will switch te LED on and setting it low will switch it off.

Where can I find information about the GPIO capabilities of a device?
---------------------------------------------------------------------
The manufacturer's hardware specification or web site information. You'll find information relating to microbit here: https://tech.microbit.org/hardware/edgeconnector/ 

Drilling down further, the hardware schematic for microbit: https://github.com/bbcmicrobit/hardware/blob/master/SCH_BBC-Microbit_V1.3B.pdf shows us on sheet 5 that the microbit contains a hardware module called an nRF51822 from a company called Nordic Semiconductor. We can see that it is this module which hosts the GPIO pins, labelled P0.nn on the diagram.

Information about the nRF51822 is contained within its datasheet (and elsewhere). See https://infocenter.nordicsemi.com/pdf/nRF51822_PS_v3.1.pdf Section 3.7 covers GPIO.

The nRF51822 contans many components of interest, key of which is an Arm Cortext-M0 CPU and this is where code gets executed. Note also that an integral Analogue to Digital Converter (ADC) makes it possible to use GPIO with analogue values, not just digital on/off states.

How does GPIO work?
-------------------
GPIO, along with other aspects of a microcontroller is memory mapped. Each pin is associated with a special memory address, typically a register. Usually, a collection of bits at that memory address, referred to as a "PORT" will each correspond to the state of an associated GPIO pin. 

There are two ways in which you can use GPIO pins. You can either poll the state of the pin of interest within the appropriate port, i.e. read it every so often in a loop or.... you can leverage interrupts.

Interrupts are a fundamental capability of computers. They allow events such as changes in state of connected peripherals to be immediately serviced by the computer and its program. When an event takes place (e.g. a button is pressed), an interrupt is generated, signalling to the CPU that something has happened. The CPU will suspend execution of the current code and briefly hand control to some other code, known as an Interrupt Service Routine (ISR). ISRs must execute in a very short time and care must be taken to ensure your ISR does not take too long to complete. After the ISR has completed, the CPU will switch back to executing the main code, from the point where it was suspended to handle the interrupt.  

References
----------

* https://en.wikipedia.org/wiki/Memory-mapped_I/O
* https://en.wikipedia.org/wiki/Processor_register
* https://en.wikipedia.org/wiki/Interrupt
* https://tech.microbit.org/hardware/edgeconnector/
* https://github.com/bbcmicrobit/hardware/blob/master/SCH_BBC-Microbit_V1.3B.pdf
* https://infocenter.nordicsemi.com/pdf/nRF51822_PS_v3.1.pdf
