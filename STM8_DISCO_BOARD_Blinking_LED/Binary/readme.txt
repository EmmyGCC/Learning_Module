/** @page DemoFW_Update How to use STVP to update the Demonstration FW

  @verbatim
  ******************** (C)COPYRIGHT 2017 STMicroelectronics *******************
  * @file     readme.txt
  * @author   MCD Application Team
  * @version  V1.0.0
  * @date     29-December-2017
  * @brief    How to use STVP to update the Demonstration FW
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
   @endverbatim
  
@par How to use it?

 To upgrade the demonstration firmware running on your board, go through the 
 following steps:

 - Power on the STM8-SO8-DISCO board and connect it to your hardware programming tool.
 - Open STVP and proceed as follows:
   - Configure the ST Visual Programmer: In the 'Configure' menu, click 
      'Configure ST Visual Programmer'. 
      The Configuration dialog box opens. 
      - From the Hardware list, select the hardware you are using (STICE, RLink or 
      ST_LINK)
      - From the Programming mode list, select SWIM.     
      - From the Device list, select STM8S001J3 or STM8L001J3 or STM8L050J3.
      - Click OK to save your changes and close the dialog box. 
   - Load the image STM8X0XXJ3_Blinking_LED.s19 in the appropriate STM8 product:
      - Select 'Program MEMORY'table
      - Open the STM8S001J3_Blinking_LED.s19 or STM8L050J3_Blinking_LED.s19
              or STM8L001J3_Blinking_LED.s19 image file
      - Program the FLASH Program Memory table: In the Program menu, Click 'Current tab'   
 - The demonstration firmware is now programmed and you can close the STVP.

 * <h3><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h3>
 */