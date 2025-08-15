#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>

class GoogleAdsInjector {
private:
    std::string adSenseCode;
    std::string adClientId;
    std::string adSlotId;

public:
    // Constructor with default Google AdSense template
    GoogleAdsInjector(const std::string& clientId, const std::string& slotId) 
        : adClientId(clientId), adSlotId(slotId) {
        generateAdCode();
    }
    
    // Generate Google AdSense ad code
    void generateAdCode() {
        adSenseCode = R"(
<!-- Google AdSense Ad -->
<script async src="https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js?client=)" + adClientId + R"("
     crossorigin="anonymous"></script>
<ins class="adsbygoogle"
     style="display:block"
     data-ad-client=")" + adClientId + R"("
     data-ad-slot=")" + adSlotId + R"("
     data-ad-format="auto"
     data-full-width-responsive="true"></ins>
<script>
     (adsbygoogle = window.adsbygoogle || []).push({});
</script>
<!-- End Google AdSense Ad -->
)";
    }
    
    // Generate responsive banner ad
    std::string generateBannerAd() {
        return R"(
<!-- Google AdSense Banner Ad -->
<script async src="https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js?client=)" + adClientId + R"("
     crossorigin="anonymous"></script>
<ins class="adsbygoogle"
     style="display:block"
     data-ad-client=")" + adClientId + R"("
     data-ad-slot=")" + adSlotId + R"("
     data-ad-format="banner"
     data-full-width-responsive="true"></ins>
<script>
     (adsbygoogle = window.adsbygoogle || []).push({});
</script>
<!-- End Google AdSense Banner Ad -->
)";
    }
    
    // Generate square ad
    std::string generateSquareAd() {
        return R"(
<!-- Google AdSense Square Ad -->
<script async src="https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js?client=)" + adClientId + R"("
     crossorigin="anonymous"></script>
<ins class="adsbygoogle"
     style="display:inline-block;width:300px;height:250px"
     data-ad-client=")" + adClientId + R"("
     data-ad-slot=")" + adSlotId + R"("></ins>
<script>
     (adsbygoogle = window.adsbygoogle || []).push({});
</script>
<!-- End Google AdSense Square Ad -->
)";
    }
    
    // Read HTML file
    std::string readFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        return content;
    }
    
    // Write HTML file
    void writeFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not create file: " + filename);
        }
        
        file << content;
        file.close();
    }
    
    // Insert ad after opening body tag
    std::string insertAdAfterBodyOpen(const std::string& html) {
        std::regex bodyRegex("(<body[^>]*>)", std::regex_constants::icase);
        return std::regex_replace(html, bodyRegex, "$1\n" + adSenseCode);
    }
    
    // Insert ad before closing body tag
    std::string insertAdBeforeBodyClose(const std::string& html) {
        std::regex bodyRegex("(</body>)", std::regex_constants::icase);
        return std::regex_replace(html, bodyRegex, adSenseCode + "\n$1");
    }
    
    // Insert ad after specific heading tags
    std::string insertAdAfterHeadings(const std::string& html, const std::string& headingTag = "h2") {
        std::string pattern = "(</" + headingTag + ">)";
        std::regex headingRegex(pattern, std::regex_constants::icase);
        return std::regex_replace(html, headingRegex, "$1\n" + adSenseCode);
    }
    
    // Insert ad after every N paragraphs
    std::string insertAdAfterParagraphs(const std::string& html, int paragraphCount = 3) {
        std::string result = html;
        std::regex pRegex("</p>", std::regex_constants::icase);
        std::sregex_iterator iter(result.begin(), result.end(), pRegex);
        std::sregex_iterator end;
        
        int count = 0;
        std::vector<size_t> positions;
        
        // Find positions where we want to insert ads
        for (std::sregex_iterator i = iter; i != end; ++i) {
            count++;
            if (count % paragraphCount == 0) {
                positions.push_back(i->position() + i->length());
            }
        }
        
        // Insert ads from back to front to maintain correct positions
        for (auto it = positions.rbegin(); it != positions.rend(); ++it) {
            result.insert(*it, "\n" + adSenseCode);
        }
        
        return result;
    }
    
    // Insert multiple ad types at different positions
    std::string insertMultipleAds(const std::string& html) {
        std::string result = html;
        
        // Add banner ad after body opens
        result = insertAdAfterBodyOpen(result);
        
        // Add square ads after every 2 h2 tags
        std::string squareAd = generateSquareAd();
        std::regex h2Regex("(</h2>)", std::regex_constants::icase);
        int h2Count = 0;
        std::string tempResult = result;
        std::sregex_iterator iter(tempResult.begin(), tempResult.end(), h2Regex);
        std::sregex_iterator end;
        
        std::vector<size_t> h2Positions;
        for (std::sregex_iterator i = iter; i != end; ++i) {
            h2Count++;
            if (h2Count % 2 == 0) {
                h2Positions.push_back(i->position() + i->length());
            }
        }
        
        for (auto it = h2Positions.rbegin(); it != h2Positions.rend(); ++it) {
            result.insert(*it, "\n" + squareAd);
        }
        
        // Add banner ad before body closes
        result = insertAdBeforeBodyClose(result);
        
        return result;
    }
};

int main() {
    try {
        // Replace with your actual Google AdSense client ID and ad slot ID
        std::string clientId = "ca-pub-XXXXXXXXXXXXXXXX";  // Your AdSense client ID
        std::string slotId = "XXXXXXXXXX";                  // Your ad slot ID
        
        std::cout << "Google AdSense HTML Injector\n";
        std::cout << "==========================\n\n";
        
        // Get input and output filenames
        std::string inputFile, outputFile;
        std::cout << "Enter input HTML file name: ";
        std::getline(std::cin, inputFile);
        
        std::cout << "Enter output HTML file name: ";
        std::getline(std::cin, outputFile);
        
        // Get AdSense details
        std::cout << "Enter your Google AdSense client ID (ca-pub-XXXXXXXXXXXXXXXX): ";
        std::getline(std::cin, clientId);
        
        std::cout << "Enter your ad slot ID: ";
        std::getline(std::cin, slotId);
        
        // Create injector instance
        GoogleAdsInjector injector(clientId, slotId);
        
        // Read HTML file
        std::string htmlContent = injector.readFile(inputFile);
        std::cout << "\nHTML file read successfully.\n";
        
        // Show options
        std::cout << "\nSelect ad insertion option:\n";
        std::cout << "1. Insert ad after <body> tag\n";
        std::cout << "2. Insert ad before </body> tag\n";
        std::cout << "3. Insert ads after every 3 paragraphs\n";
        std::cout << "4. Insert ads after h2 headings\n";
        std::cout << "5. Insert multiple ads (recommended)\n";
        std::cout << "Enter choice (1-5): ";
        
        int choice;
        std::cin >> choice;
        
        std::string modifiedHtml;
        
        switch(choice) {
            case 1:
                modifiedHtml = injector.insertAdAfterBodyOpen(htmlContent);
                break;
            case 2:
                modifiedHtml = injector.insertAdBeforeBodyClose(htmlContent);
                break;
            case 3:
                modifiedHtml = injector.insertAdAfterParagraphs(htmlContent, 3);
                break;
            case 4:
                modifiedHtml = injector.insertAdAfterHeadings(htmlContent, "h2");
                break;
            case 5:
                modifiedHtml = injector.insertMultipleAds(htmlContent);
                break;
            default:
                std::cout << "Invalid choice. Using option 1.\n";
                modifiedHtml = injector.insertAdAfterBodyOpen(htmlContent);
                break;
        }
        
        // Write modified HTML to output file
        injector.writeFile(outputFile, modifiedHtml);
        
        std::cout << "\nGoogle ads have been successfully added to " << outputFile << "\n";
        std::cout << "Remember to:\n";
        std::cout << "1. Replace the client ID and slot ID with your actual AdSense details\n";
        std::cout << "2. Test the ads on your website\n";
        std::cout << "3. Ensure your website complies with Google AdSense policies\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

// Compilation instructions:
// g++ -std=c++11 -o html_ads_injector html_ads_injector.cpp
// 
// Usage:
// ./html_ads_injector
