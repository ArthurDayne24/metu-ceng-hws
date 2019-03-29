public class CengHashRow {

    public int bucketIndex;
    public int bits;
    public String prefix;
	// GUI-Based Methods
	// These methods are required by GUI to work properly.
	
    public CengHashRow(int bucketIndex, int bits, String prefix)
    {
        this.bucketIndex = bucketIndex;
        this.bits = bits;
        this.prefix = prefix;
    }

    public void setBucketIndex(int bucketIndex)
    {
        this.bucketIndex = bucketIndex;
    }

    public int getBucketIndex()
    {
//        System.out.println("TEST: bucketIndex: " + bucketIndex + " prefix: "+ prefix);
        return bucketIndex;
    }

	public String hashPrefix()
	{
		// DID: Return row's hash prefix (such as 0, 01, 010, ...)
		return prefix.equals("") ? "0" : prefix;
	}
	
	public CengBucket getBucket()
	{
		// DID: Return the bucket that the row points at.
		return CengCoinExchange.getBucketList().bucketAtIndex(bucketIndex);
	}
	
	public boolean isVisited()
	{
		// SKIP: Return whether the row is used while searching.
		return false;		
	}
}
