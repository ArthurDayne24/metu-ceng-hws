import java.util.ArrayList;

public class CengBucket {

    public int capacity;
    public int bits;
    public int pointing;
    public int npointing;
	public ArrayList<CengCoin> coins;
	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
    public CengBucket(int capacity, int bits)
    {
        this.capacity = capacity;
        this.bits = bits;

        notifyNoOnePoints();
        coins = new ArrayList<CengCoin>(this.capacity);
    }

    public void notifyNoOnePoints()
    {
        pointing = -1;
        npointing = 0;
    }

    public void notify(int index)
    {
        // hashRow at index is now pointing me
        if (pointing == -1 || pointing > index)
        {
            pointing = index;
        }
        npointing++;
    }

    public Boolean addSucceeds(CengCoin coin)
    {
        if (capacity == coins.size())
        {
            return false;
        }
       // else if (capacity < coins.size())
        //{
       //     System.out.println("++++++++++++++++++++++error+++++++++++++++++++++++++++++++++++++");
          //  return false;
        //}

        coins.add(coin);
        return true;
    }

    public Boolean isAvail()
    {
        return capacity > coins.size();
    }

	public int coinCount()
	{
		// DID: Return the coin count in the bucket.
		return coins.size();
	}
	
	public CengCoin coinAtIndex(int index)
	{
		// DID: Return the corresponding coin at the index.
		return coins.get(index);
	}
	
	public int getHashPrefix()
	{
		// DID: Return hash prefix length.
		return bits;
	}
	
	public Boolean isVisited()
	{
		// SKIP: Return whether the bucket is found while searching.
		return false;		
	}
	
}
