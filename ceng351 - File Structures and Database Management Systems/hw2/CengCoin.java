public class CengCoin {
	
	private Integer key;
	
	private String name;
	private String value;
	private String currency;
	
	public CengCoin(Integer key, String name, String value, String currency)
	{
		this.key = key;
		this.name = name;
		this.value = value;
		this.currency = currency;
	}
	
	// Getters
	
	public Integer key()
	{
		return key;
	}
	public String name()
	{
		return name;
	}
	public String value()
	{
		return value;
	}
	public String currency()
	{
		return currency;
	}
		
	// GUI method - do not modify
	public String fullName()
	{
		return "" + key() + "|" + name() + "|" + value() + "|" + currency;
	}

	// Own Methods
}
