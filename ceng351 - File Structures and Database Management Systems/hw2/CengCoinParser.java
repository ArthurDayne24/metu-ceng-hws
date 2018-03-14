import java.io.IOException;
import java.util.ArrayList;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.InputStreamReader;
import java.nio.charset.Charset; 
import java.lang.*;

public class CengCoinParser {

	public static ArrayList<CengCoin> parseCoinsFromFile(String filename)
	{
		ArrayList<CengCoin> coinList = new ArrayList<CengCoin>();
				
		// You need to parse the input file in order to use GUI tables.
		// DID: Parse the input file, and convert them into CengCoins
        
        // Format is list / coinKey / coinName / coinValue / coinCur 
        
        String curline;
        String[] parsed;
        
        try {
            BufferedReader br = new BufferedReader(new FileReader(filename));
            while ((curline = br.readLine()) != null) {
                parsed = curline.split("\\|");
                coinList.add(new CengCoin(Integer.parseInt(parsed[1]), parsed[2],
                            parsed[3], parsed[4]));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

		return coinList;
	}
	
	public static void startParsingCommandLine() throws IOException
	{
		// DID: Start listening and parsing command line -System.in-.
		// There are 4 commands:
		// 1) quit : End the app, gracefully. Print nothing, call nothing.
		// 2) add : Parse and create the coin, and call CengCoinExchange.addCoin(newlyCreatedCoin).
		// 3) search : Parse the key, and call CengCoinExchange.searchCoin(parsedKey).
		// 4) print : Print the whole hash table with the corresponding buckets, call CengCoinExchange.printEverything().

		// Commands (quit, add, search, print) are case-insensitive.
        
        String[] parsed;
        String curline;

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in, Charset.defaultCharset()));

        for (;;) {
            curline = reader.readLine();

            if (curline.startsWith("quit")) {
                System.exit(0);
            }
            // Format is list / coinKey / coinName / coinValue/ coinCur 
            else if (curline.startsWith("add")) {
                parsed = curline.split("\\|");
                CengCoinExchange.addCoin(new CengCoin(Integer.parseInt(parsed[1]), parsed[2],
                            parsed[3], parsed[4]));
            }
            else if (curline.startsWith("search")) {
                parsed = curline.split("\\|");
                CengCoinExchange.searchCoin(Integer.parseInt(parsed[1]));
            }
            else if (curline.startsWith("print")) {
                CengCoinExchange.printEverything();
            }
        }
	}
}
