function doPost(e) {
  try {
    if (e === undefined) {
      throw new Error("No POST data received.");
    }

    var sheet = SpreadsheetApp.openById("1R9hf20oeZePFUqo9AKtBOlmHQdAQoBz0z6cr_y7pxZw").getActiveSheet();
    var data = JSON.parse(e.postData.contents);

    // Append the new data to the next available row
    var value1 = parseFloat(data.value1) * 0.000125; // Value for column 1 (handle negative and floating-point numbers)
    var value2 = parseFloat(data.value2) * 0.1095;// Value for column 2
    sheet.appendRow([value1, value2]);

    // Calculate the sum of all rows in columns 1 and 2 starting from the 2nd row
    var totalValue1 = 0;
    var totalValue2 = 0;
    
    var lastRow = sheet.getLastRow();

    // Loop through all rows starting from the 2nd row (skip the header)
    for (var i = 2; i <= lastRow; i++) {
      var rowValue1 = sheet.getRange(i, 1).getValue(); // Get value from column 1
      var rowValue2 = sheet.getRange(i, 2).getValue(); // Get value from column 2

      // Accumulate the sum with the respective multipliers (handling negative and large numbers)
      totalValue1 += rowValue1;
      totalValue2 += rowValue2; 
    }

    // Write the sum back to the second row (sum of all data from row 2 and below)
    sheet.getRange(2, 1).setValue(totalValue1); // Sum of column 1
    sheet.getRange(2, 2).setValue(totalValue2); // Sum of column 2

    // Delete all rows except the first (header) and second (sum row)
    if (lastRow > 2) {
      sheet.deleteRows(3, lastRow - 2); // Remove rows from the 3rd row onward
    }

    return ContentService.createTextOutput("Success");
  } catch (error) {
    Logger.log("Error: " + error.message);
    return ContentService.createTextOutput("Error: " + error.message);
  }
}
