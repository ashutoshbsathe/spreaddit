# spreaddit\[Beta\]
### A simple and lightweight spreadsheet editor fully written in C!
Spreaddit = Spreadsheet Edit

Spreaddit allows you to :
- Open and edit `.csv` files
- Open and edit `.ods` files
- Evaluate expressions based on cell data
- Sort the given file on a particular column
- Allow you to change date format in the spreadsheet
- Save the modified file in `.csv` format

### Dependencies :
**GTK+3** : This  project uses GTK+3 to  build GUI. Recommended version is 3.18.9.<br>
> `sudo apt-get install libgtk-3-dev` for debian based systems

**LibXML2** : Required for parsing the `content.xml` file from the extracted `.ods` file. Recommended version is 2.9.3<br>
> `sudo apt-get install libxml2-dev` for debian based systems

## Overview :

### Application Panel :
![Application Panel](https://i.imgur.com/m0Y3S5l.png)

All the things described below are just sake of the documentation which is required for evaluation of this project for the DSA project exam.

### Formula Bar and Message Window :
![Formula Bar and Message Window](https://i.imgur.com/agTMeVX.png)

These two are the very important parts of this application window. You will get important notifications such as coordinates of the active cell in message window. Any error will also be flashed with yellow background in message window. <br>
The formula bar can be used to edit the content of the active cell. After editing the content in the formula bar, click on _Apply_ button to reflect them in the active cell. You can write mathematical expressions involving varios cells in the formula bar.<br>
For e.g.<br>
> `A1 + B1 + C1`<br> `=A1 + 1024` 

If the first character of your edited formula bar content is `=` and you don't want to evaluate it as formula, put the formula between the double quotes as follows :

> `"A1 + B1"`
#### Special Formulae : 
Use `=sort(ASC)` or `=sort(DESC)` in a cell to sort the part of active column below the selected cell in ascending or descending order respectively. If you want to sort on entire column, you must put this formula in the topmost cell of that particular column.

### Style Buttons :
| Button | Function  |
| ------ | --------- |
|![Bold Button](https://i.imgur.com/uzWroUt.png) | Make the content of the selected cell **bold** |
|![Italics Button](https://i.imgur.com/wSlvJ3z.png) | Make the content of the selected cell _italic_ |

### Open and Save Buttons :
 ![Open Buttons](https://i.imgur.com/hCHaCXC.png)  ![Save Button](https://i.imgur.com/DdCPNhJ.png) 

Open buttons are for opening the specified type of file. Click on the respective button to open either `.ods` or `.csv` file. Please note that opening a `.ods` file is still under testing. Additionally there are only specific types of `.ods` files that are readable via the program.
- `.ods` file should contain only one spreadsheet
- `.ods` file should not contain any styling that affects the data representation
- `.ods` file should not contain any media (graphs, macros etc.) 
- `.ods` file should not have merged cells or columns(because my GUI does not support that yet)

Save button can be used to save the current file in `.csv` format. Before saving for the first time you will be asked to enter filename. All the subsequent saves/writes will be done to the same file that was specified in the first attempt. It is however important to make sure that filename also contains the extension `.csv`, the program will not handle the incomplete/unknown extensions.

### Insert Rows and Columns :
| Button | Function |
| ------ | -------- |
| ![Insert Row Below](https://i.imgur.com/dvKCezO.png)  | Insert Row Below |
| ![Insert Row Above](https://i.imgur.com/5Otys0l.png) | Insert Row Above |
| ![Insert Column Left](https://i.imgur.com/VfHc519.png) | Insert Columns Left |
| ![Insert Column Right](https://i.imgur.com/acf61vA.png) | Insert Row Right |

### Delete Rows and Columns :
![Delete Row or Column](https://i.imgur.com/gXwFP5r.png)

Select any cell in a row/column that you want to delete and then use these buttons to delete the particular row/column.

### Date Formatting Buttons :
![Date Formatting Buttons](https://i.imgur.com/Z7ab6GJ.png)

Select a cell containing the date and click on the desired date formatting button to change its format. 

(Icons used in this project are taken from libreoffice wiki along with default icons in gtk library)<br>
Main Icon Inspiration : http://www.iconarchive.com/show/filetype-icons-by-graphicloads/csv-icon.html

<!-- End README -->
