import java.util.ArrayList;

public class CengHashTable {

    public int hashMod;
    public int bits;
    public int totalBits;
    public int bucketCapacity;
    public CengBucketList bucketlist;
    public ArrayList<CengHashRow> hashRows;

    public CengHashTable()
    {
        // DID: Create a hash table with only 1 row.
        this.hashMod = CengCoinExchange.getHashMod();
        this.bits = 0;
        this.totalBits = Integer.toBinaryString(hashMod-1).length();
        this.bucketCapacity = CengCoinExchange.getBucketSize();
        this.bucketlist = CengCoinExchange.getBucketList();
        this.hashRows = new ArrayList<CengHashRow>(1);

        this.bucketlist.addBucket(0);
        this.bucketlist.bucketAtIndex(0).notify(0);

        // bucketIndex, bits, prefix
        this.hashRows.add(new CengHashRow(0, 0, ""));
    }

    public CengBucket getRowsBucket(int rowIndex)
    {
        //CengBucket b = bucketlist.bucketAtIndex(rowAtIndex(rowIndex).getBucketIndex());
        CengBucket b = rowAtIndex(rowIndex).getBucket();
//        System.out.println("hashtableTEST: bucketIndex: " + rowAtIndex(rowIndex).getBucketIndex() + " prefix: "+ rowAtIndex(rowIndex).prefix);

        return b;
    }

    private void extend()
    {
        // System.out.println("bits: " + bits + "rows: " + hashRows.size() + "  --------before extend--------------");
        //for (CengHashRow cr: hashRows)
        //{
            // System.out.println("++++++++++++++++++++++++-");
            // System.out.println("bucketindex: " + cr.bucketIndex);
            // System.out.println("bits: " + cr.bits);
            // System.out.println("prefix: " + cr.prefix);
            // System.out.println("++++++++++++++++++++++++-");
        //}

        ArrayList<CengHashRow> trows = new ArrayList<CengHashRow>(hashRows.size()*2);
        bits++;

        for (int i = 0; i < hashRows.size(); i++)
        {
            CengBucket bucket = getRowsBucket(i);
            bucket.notifyNoOnePoints();
        }
        // System.out.println("In extent");
        for (int i = 0; i < hashRows.size(); i++)
        {
            CengBucket bucket = getRowsBucket(i);
            CengHashRow oldRow = rowAtIndex(i);

            trows.add(oldRow);
            
            trows.add(new CengHashRow(oldRow.getBucketIndex(), oldRow.bits, oldRow.prefix+"1"));

            // WARNING: BAD DESIGN / PROBLEMATIC
            trows.get(2*i).prefix += "0";
            
            bucket.notify(2*i);
            bucket.notify(2*i+1);
        }
        hashRows = trows;

//        System.out.println("After exten++++++++++++++++++++++++++d");


       // for (int i = 0; i < hashRows.size(); i++)
        //{
         //   System.out.println(hashRows.get(i).bucketIndex);
          //  CengBucket bucket = getRowsBucket(i);
  //          System.out.println("pointing: " + bucket.pointing + "\nnpointing: " + bucket.npointing);
    //        System.out.println("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
       // }

        // System.out.println("bits: " + bits + "rows: " + hashRows.size() + "----------after extend--------------");
        //for (CengHashRow cr: hashRows)
        //{
            // System.out.println("++++++++++++++++++++++++-");
            // System.out.println("bucketindex: " + cr.bucketIndex);
            // System.out.println("bits: " + cr.bits);
            // System.out.println("prefix: " + cr.prefix);
            // System.out.println("++++++++++++++++++++++++-");
        //}

    }

    public CengBucket getBucket(CengCoin coin)
    {
        return getRowsBucket(computeIndex(coin));
    }

    public Boolean isSplittable(CengBucket bucket)
    {
        return bits > bucket.bits;
    }

    public int computeIndex(CengCoin coin)
    {
        return (coin.key() % hashMod) >>> (totalBits - bits);
    }

    public int computeIndex(int key)
    {
        return (key % hashMod) >>> (totalBits - bits);
    }

    public void splitBucket(CengBucket bucket)
    {

        // System.out.println("Enter split!");
        bucketlist.disableOneBucket();

        int bucketIndex1 = bucketlist.addBucket(bucket.bits+1);
        for (int i=bucket.pointing; i<bucket.pointing+bucket.npointing/2; i++)
        {
            rowAtIndex(i).setBucketIndex(bucketIndex1);
            bucketlist.bucketAtIndex(bucketIndex1).notify(i);
            rowAtIndex(i).bits = bucket.bits+1;
        }

        int bucketIndex2 = bucketlist.addBucket(bucket.bits+1);
        for (int i=bucket.pointing+bucket.npointing/2; i<bucket.pointing+bucket.npointing; i++)
        {
            rowAtIndex(i).setBucketIndex(bucketIndex2);
            bucketlist.bucketAtIndex(bucketIndex2).notify(i);
            rowAtIndex(i).bits = bucket.bits+1;
        }

        for (CengCoin coin: bucket.coins)
        {
            addCoin(coin);
        }

//        bucket = new CengBucket(0, 0); //TODO
        
        // System.out.println("After split; bits = " + bits + " and rows.size = " + hashRows.size());
//        for (CengHashRow cr: hashRows)
  //      {
            // System.out.println("++++++++++++++++++++++++");
            // System.out.println("bucketindex: " + cr.bucketIndex);
            // System.out.print("For that bucket\n");
    //        for (CengCoin c: bucketlist.bucketAtIndex(cr.bucketIndex).coins)
      //      {
                // System.out.println("\t"+c.key());
        //    }
            // System.out.println("bits: " + cr.bits);
            // System.out.println("prefix: " + cr.prefix);
            // System.out.println("++++++++++++++++++++++++-");
       // }
    }

    public void addCoin(CengCoin coin)
    {           
        // System.out.println(146);
        CengBucket bucket = getBucket(coin);
        // System.out.println(coin.key() + " is now being added...");

        if (!bucket.addSucceeds(coin))
        {
            // System.out.println(151);
            if (isSplittable(bucket))
            {
                // System.out.println(154);
                // System.out.println("I should be there when 2!");
                splitBucket(bucket);
                // System.out.println(157);
                addCoin(coin);
                // System.out.println(159);
            }
            else
            {
                // System.out.println(163);
                extend();
                // System.out.println(165);
                addCoin(coin);
                // System.out.println(167);
            }
        }
    }
    
    public void searchCoin(Integer key)
    {
        // DID: Empty Implementation
        String rowsText = "";

        CengBucket bucket = getRowsBucket(computeIndex(key));
        boolean found = false;
        for (CengCoin coin: bucket.coins)
        {
            if (coin.key().equals(key))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            System.out.println("BEGIN SEARCH\nNone\nEND SEARCH");
            return;
        }

        for (int rowIdx = bucket.pointing; rowIdx < bucket.pointing+bucket.npointing; rowIdx++)
        {
            rowsText += hashRow2Str(hashRows.get(rowIdx), rowIdx, 0);
        }

        String text =
            "BEGIN SEARCH\n"+
            rowsText+
            "END SEARCH";

        System.out.println(text);
    }
    
    public void print()
    {
        // DID: Empty Implementation
        String rowsText = "";
        for (int rowIdx = 0; rowIdx < hashRows.size(); rowIdx++)
        {
            CengHashRow row = hashRows.get(rowIdx);
            rowsText += hashRow2Str(row, rowIdx, 0);
        }

        String text =
            "BEGIN TABLE\n"+
            rowsText+
            "END TABLE";

         System.out.println(text);
    }

    // GUI-Based Methods
    // These methods are required by GUI to work properly.
    
    public int prefixBitCount()
    {
        // DID: Return table's hash prefix length.
        return bits;        
    }
    
    public int rowCount()
    {
        // DID: Return the count of HashRows in table.
        return hashRows.size();
    }
    
    public CengHashRow rowAtIndex(int index)
    {
        // DID: Return corresponding hashRow at index.
        return hashRows.get(index);
    }

    public String coin2Str(CengCoin coin, String prefix, int tabs)
    {
        String coinHash = Integer.toBinaryString(coin.key() % hashMod);
        String padding = "";
        for (int i = 0; i < totalBits - coinHash.length(); i++)
        {
            padding += "0";
        }

        coinHash = padding + coinHash ;       

        String innerTabs = generateTabs(tabs);
        String text = 
            innerTabs+"<coin>\n"+
                innerTabs+"\t<hash>"+coinHash+"</hash>\n"+
                innerTabs+"\t<value>"+coin.key()+"|"+coin.name()+"|"+coin.value()+
                    "|"+coin.currency()+"</value>\n"+
            innerTabs+"</coin>\n";

        return text;
    }

    public String bucket2Str(CengBucket bucket, String prefix, int tabs)
    {
        String coinsText = "";
        for (CengCoin coin: bucket.coins)
        {
            coinsText += coin2Str(coin, prefix, tabs+1);
        }
        
        String innerTabs = generateTabs(tabs);

//        String pointings = "-> "; // 
//        for (int i = bucket.pointing; i < bucket.pointing+bucket.npointing; i++) //
 //           pointings += i+","; //
        String text =
   //         innerTabs+"<bucket>"+pointings+"\n" +// 
            innerTabs+"<bucket>\n"+ // 
                innerTabs+"\t<hashLength>"+bucket.bits+"</hashLength>\n"+
                coinsText+
            innerTabs+"</bucket>\n";

        return text;
    }

    public String hashRow2Str(CengHashRow cengHashRow, int rowIndex, int tabs)
    {
        String bucketText = bucket2Str(getRowsBucket(rowIndex), cengHashRow.prefix, tabs+1);

        String innerTabs = generateTabs(tabs);
        String trick = cengHashRow.prefix.equals("") ? "0" : cengHashRow.prefix;
        String text =
//          innerTabs+"<row>myBucket: " + cengHashRow.bucketIndex+"\n"+
            innerTabs+"<row>\n"+
                innerTabs+"\t<hashPrefix>"+trick+"</hashPrefix>\n"+
                bucketText+
            innerTabs+"</row>\n";

        return text;
    }

    public String generateTabs(int tabs)
    {
        String innerTabs = "";
        for (int i = 0; i < tabs; i++)
        {
            innerTabs += "\t";
        }
        return innerTabs;
    }
}
