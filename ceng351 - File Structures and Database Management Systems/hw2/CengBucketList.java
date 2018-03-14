import java.util.ArrayList;

public class CengBucketList {

	public ArrayList<CengBucket> buckets;
    public int capacity;
    public int bcount;

    public CengBucketList()
    {
        this.capacity = CengCoinExchange.getBucketSize();
        this.buckets = new ArrayList<CengBucket>();
    }

	public void addCoin(CengCoin coin)
	{
		// SKIP: Empty Implementation
	}
	
	public void searchCoin(Integer key)
	{
		// SKIP: Empty Implementation
	}
	
	public void print()
	{
		// SKIP: Empty Implementation
	}
	
	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
    public void disableOneBucket()
    {
        bcount--;
    }

    public int addBucket(int bits)
    {
        bcount++;
        buckets.add(new CengBucket(capacity, bits));
        return buckets.size() - 1;
    }

	public int bucketCount()
	{
		// DID: Return all bucket count.
		return bcount;
	}
	
	public CengBucket bucketAtIndex(int index)
	{
		// DID: Return corresponding bucket at index.
		return buckets.get(index);
	}
}
