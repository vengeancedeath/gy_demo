---
pagetitle: Release Notes for STM32F0xx-Nucleo  Board Drivers
lang: en
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

::: {.card .fluid}
::: {.sectione .dark}
<center>
# **Release Notes for STM32F0xx-Nucleo BSP Drivers**
Copyright &copy; 2016 STMicroelectronics\
    
[![ST logo](_htmresc/st_logo.png)](https://www.st.com){.logo}
</center>
:::
:::

# Purpose

The BSP (Board Specific Package) drivers are parts of the STM32Cube package based on the HAL drivers and provide a set of high level APIs relative to the hardware components and features in the evaluation boards, discovery kits and nucleo boards coming with the STM32Cube package for a given STM32 series.


The BSP drivers allow a quick access to the boards’ services using high level APIs and without any specific configuration as the link with the HAL and the external components is done in intrinsic within the drivers. 


From project settings points of view, user has only to add the necessary driver’s files in the workspace and call the needed functions from examples. However some low level configuration functions are weak and can be overridden by the applications if user wants to change some BSP drivers default behavior.

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section9" checked aria-hidden="true">
<label for="collapse-section9" aria-hidden="true">__V1.1.7 / 19-December-2022__</label>
<div>

## Main Changes

-  All source files: update disclaimer to add reference to the new license agreement.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">__V1.1.6 / 25-August-2017__</label>
<div>

## Main Changes

-  Minor updates in STM32F0xx-Nucleo_BSP_User_Manual.chm documentation.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section7" aria-hidden="true">
<label for="collapse-section7" aria-hidden="true">__V1.1.5 / 7-April-2017__</label>
<div>

## Main Changes

-  Updated BSP driver documentation.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section6" aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">__V1.1.4 / 04-November-2016__</label>
<div>

## Main Changes

-  Update CHM User Manual for BSP/STM32F0xx_Nucleo.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5" aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">__V1.1.3 / 29-January-2016__</label>
<div>

## Main Changes

-  Updated GPIO Output Speed literals naming to ensure HAL full compatibility.
-  Added new services for SD management, as required by updated version of Adafruit 1.8 component.
-  Modified GPIO_LED configuration to NOPULL (to improve power consumption).
-  Modified USER_BUTTON definitions to ensure compatibility between Nucleo boards

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">__V1.1.2 / 26-June-2015__</label>
<div>

## Main Changes

-  Update CHM User Manual for BSP/STM32F0xx_Nucleo.
-  Complete HAL API alignment (renaming).
-  Add missing ADC calibration for JoyStick management.
-  Update ADC polling in Nucleo shield Adafruit joystick.
-  Implement workaround to cover RCC limitation regarding Peripheral enable delay.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">__V1.1.1 / 27-November-2014__</label>
<div>			

## Main Changes

-	Case mismatch between #include typo & effective file name generating compiler errors under Linux.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V1.1.0 / 01-August-2014__</label>
<div>			

## Main Changes

-	Compatibility with LCD Component driver update:
    -	Change LCD_IO_WriteData to LCD_IO_WriteMultipleData
    -	LCD IO needs to treat also 1 byte.
-	New supported devices are STM32F091RC.

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V1.0.0 / 20-May-2014__</label>
<div>			

## Main Changes

-   First official release dedicated to **STM32CubeF0 based development**. Supported devices are STM32F072RB and STM32F030R8.


</div>
:::

:::
:::

<footer class="sticky">
For complete documentation on <mark>STM32 Microcontrollers</mark> ,
visit: [[www.st.com](http://www.st.com/STM32)]{style="font-color: blue;"}
</footer>
