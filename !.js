const fs = require("fs");

function replaceHeaderIncludes(filePath) {
    fs.readFile(filePath, "utf8", (err, data) => {
        if (err) {
            console.error(`Error reading file ${filePath}:`, err);
            return;
        }

        const newData = data.replace(/<openssl\/(.*\.h)>/g, '"$1"');

        fs.writeFile(filePath, newData, (err) => {
            if (err) {
                console.error(`Error writing file ${filePath}:`, err);
            } else {
                console.log(`Replaced header includes in ${filePath}`);
            }
        });
    });
}

// Get all .h files in the current directory
fs.readdir(".", (err, files) => {
    if (err) {
        console.error("Error reading directory:", err);
        return;
    }

    files.forEach((file) => {
        if (file.endsWith(".h")) {
            replaceHeaderIncludes(file);
        }
    });
});
