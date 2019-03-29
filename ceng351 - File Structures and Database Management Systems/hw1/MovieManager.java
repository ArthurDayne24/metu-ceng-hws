
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.List;
import java.sql.Statement;
import java.sql.ResultSet;
import java.util.LinkedList;

// TODO null problems

/**
 *
 * @author gorkem
 */
public class MovieManager implements IMovieManager {

    /* These values are read from databaseConfiguration.txt file. */
    private String _USERNAME;
    private String _PASSWORD;
    private String _HOST;
    private String _DATABASE_SCHEMA;
    private int _PORT_NUMBER;

    private static String _DB_CONFIG_FILE_NAME = "databaseConfiguration.txt";
    private static String _ACTOR_FILE_NAME = "actors.txt";
    private static String _MOVIE_FILE_NAME = "movies.txt";
    private static String _DIRECTOR_FILE = "directors.txt";
    private static String _USER_FILE = "users.txt";

    private Connection _connection;

    // DO NOT MODIFY THIS FUNCTION
    public void ReadDatabaseConfiguration() throws Exception {
        BufferedReader br = null;

        try {
            String currentLine;
            br = new BufferedReader(new FileReader(this._DB_CONFIG_FILE_NAME));

            while ((currentLine = br.readLine()) != null) {
                if (currentLine.startsWith("host")) {
                    this._HOST = currentLine.replace("host=", "").trim();
                }
                if (currentLine.startsWith("schema")) {
                    this._DATABASE_SCHEMA = currentLine.replace("schema=", "").trim();
                }
                if (currentLine.startsWith("username")) {
                    this._USERNAME = currentLine.replace("username=", "").trim();
                }
                if (currentLine.startsWith("password")) {
                    this._PASSWORD = currentLine.replace("password=", "").trim();
                }
                if (currentLine.startsWith("port")) {
                    this._PORT_NUMBER = Integer.parseInt(currentLine.replace("port=", "").trim());
                }
            }
        } catch (Exception ex) {
            System.out.println("Error while reading databaseConfiguration.txt file: " + ex.getMessage());
            throw ex;
        } finally {
            try {
                if (br != null) {
                    br.close();
                }
            } catch (IOException ex) {
                System.out.println("Error while reading databaseConfiguration.txt file: " + ex.getMessage());
                throw ex;
            }
        }

        System.out.println("Database configuration is read.");
    }

    // DO NOT MODIFY THIS FUNCTION
    public void InitializeConnection() {

        String url = "jdbc:mysql://" + this._HOST + ":" + this._PORT_NUMBER + "/"
                + this._DATABASE_SCHEMA + "?useUnicode=true&characterEncoding=UTF-8";

        try {
            Class.forName("com.mysql.jdbc.Driver");
            this._connection = DriverManager.getConnection(url, this._USERNAME,
                    this._PASSWORD);
        } catch (SQLException e) {
            e.printStackTrace();

        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }

        System.out.println("Database connection initialized.");
    }

    @Override
    public void ParseFilesAndInsertData() {
        
        String[] parsed;
        String currentLine;
        BufferedReader br = null;
        ResultSet rs;

        try {
            // Actor
            br = new BufferedReader(new FileReader(this._ACTOR_FILE_NAME));
            
            while ((currentLine = br.readLine()) != null) {
                parsed = currentLine.split(";");
                InsertActor(new Actor(Integer.parseInt(parsed[0]), parsed[1]));
            }

            // Director
            br = new BufferedReader(new FileReader(this._DIRECTOR_FILE));

            while ((currentLine = br.readLine()) != null) {
                parsed = currentLine.split(";");
                InsertDirector(new Director(Integer.parseInt(parsed[0]), parsed[1]));
            }

            // Movie
            br = new BufferedReader(new FileReader(this._MOVIE_FILE_NAME));

            while ((currentLine = br.readLine()) != null) {
                parsed = currentLine.split(";");

                Statement statement = _connection.createStatement();

                rs = statement.executeQuery(String.format(
                    "SELECT d.did FROM director d WHERE d.name=\"%s\";", parsed[5]));
                rs.next();

                Movie movie = new Movie(Integer.parseInt(parsed[0]), parsed[1], parsed[3],
                    Float.parseFloat(parsed[6]), Integer.parseInt(parsed[2]),
                    new Director(rs.getInt("did"), parsed[5]));

                InsertMovie(movie);
        
                // Casted_in needs actor and movies
                for (String aName: parsed[4].split(", ")) {
                    rs = statement.executeQuery(String.format(
                        "SELECT a.aid FROM actor a WHERE a.name=\"%s\";", aName));
                    rs.next();

                    InsertCastedIn(new Actor(rs.getInt("aid"), aName), movie);
                }
            }

            // User
            br = new BufferedReader(new FileReader(this._USER_FILE));

            while ((currentLine = br.readLine()) != null) {
                parsed = currentLine.split(";");
                InsertUser(new User(Integer.parseInt(parsed[0]), parsed[1],
                            Integer.parseInt(parsed[2])));

                // Watched needs users and movies
                for (String movieId: parsed[3].split(", ")) {
                    InsertWatched(new User(Integer.parseInt(parsed[0]), "", 0),
                      new Movie(Integer.parseInt(movieId), "", "", 0, 0, new Director()));
                }
            }
    
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        
        }
    }

    @Override
    public void CreateTables() {

        try {
            Statement statement = _connection.createStatement();

            // actor Table
            statement.executeUpdate(
                "CREATE TABLE actor ( " +
                  "aid INT,           " +
                  "name VARCHAR(50),  " +
                  "PRIMARY KEY(aid)   " +
                ");"
            );

            // director Table
            statement.executeUpdate(
                "CREATE TABLE director ( " +
                  "did INT,              " +
                  "name VARCHAR(50),     " +
                  "PRIMARY KEY(did)      " +
                ");"
            );

            // movie Table
            statement.executeUpdate(
                "CREATE TABLE movie (        " +
                  "mid INT,                  " +
                  "title VARCHAR(100),       " +
                  "genre VARCHAR(50),        " +
                  "year INT,                 " +
                  "did INT,                  " +
                  "rating DOUBLE,            " +
                  "PRIMARY KEY(mid),         " + 
                  "FOREIGN KEY(did)          " + 
                   "REFERENCES director(did) " +
                   "ON DELETE CASCADE        " +
                   "ON UPDATE CASCADE        " +
                ");"
            );

            // user Table
            statement.executeUpdate(
                "CREATE TABLE user ( " +
                  "uid INT,          " +
                  "name VARCHAR(50), " +
                  "age int,          " +
                  "PRIMARY KEY(uid)  " + 
                ");"
            );

            // casted_in Rel
            statement.executeUpdate(
                "CREATE TABLE casted_in (  " +
                  "aid INT,                " +
                  "mid INT,                " +
                  "PRIMARY KEY(aid, mid),  " + 
                  "FOREIGN KEY(aid)        " + 
                   "REFERENCES actor(aid)  " +
                   "ON DELETE CASCADE      " +
                   "ON UPDATE CASCADE,     " +
                  "FOREIGN KEY(mid)        " + 
                   "REFERENCES movie(mid)  " +
                   "ON DELETE CASCADE      " +
                   "ON UPDATE CASCADE      " +
                ");"
            );

            // watched Rel
            statement.executeUpdate(
                "CREATE TABLE watched ( " +
                  "uid INT,               " +
                  "mid INT,               " +
                  "PRIMARY KEY(uid, mid), " + 
                  "FOREIGN KEY(uid)       " + 
                   "REFERENCES user(uid)  " +
                   "ON DELETE CASCADE     " +
                   "ON UPDATE CASCADE,    " +
                  "FOREIGN KEY(mid)       " + 
                   "REFERENCES movie(mid) " +
                   "ON DELETE CASCADE     " +
                   "ON UPDATE CASCADE     " +
                ");"
            );

        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println(e.getMessage());

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void InsertActor(Actor actor) {

        try {
            Statement statement = _connection.createStatement();

            statement.executeUpdate(
                String.format("INSERT INTO actor VALUES (%d, \"%s\");",
                    actor.getAid(), actor.getName())
            );

        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println(e.getMessage());

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void InsertDirector(Director director) {

        try {
            Statement statement = _connection.createStatement();

            statement.executeUpdate(
                String.format("INSERT INTO director VALUES (%d, \"%s\");",
                    director.getDid(), director.getName())
            );

        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println(e.getMessage());

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void InsertMovie(Movie movie) {

        try {
            Statement statement = _connection.createStatement();

            statement.executeUpdate(
                String.format("INSERT INTO movie VALUES (%d, \"%s\", \"%s\", %d, %d, %.1f);",
                    movie.getMid(), movie.getTitle(), movie.getGenre(), movie.getYear(), 
                    movie.getDirector().getDid(), movie.getRating())
            );

        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println(String.format("SQL exception with %d - %s",
                        movie.getMid(), movie.getTitle()));
            System.out.println(e.getMessage());

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Other exception with %d - %s",
                        movie.getMid(), movie.getTitle()));
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void InsertUser(User user) {

        try {
            Statement statement = _connection.createStatement();

            statement.executeUpdate(
                String.format("INSERT INTO user VALUES (%d, \"%s\", %d);",
                    user.getUid(), user.getName(), user.getAge())
            );

        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println(String.format(e.getMessage()));

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void InsertCastedIn(Actor actor, Movie movie) {

        try {
            Statement statement = _connection.createStatement();

            statement.executeUpdate(
                String.format("INSERT INTO casted_in VALUES (%d, %d);",
                    actor.getAid(), movie.getMid())
            );

        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println(e.getMessage());

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void InsertWatched(User user, Movie movie) {

        try {
            Statement statement = _connection.createStatement();

            statement.executeUpdate(
                String.format("INSERT INTO watched VALUES (%d, %d);",
                    user.getUid(), movie.getMid())
            );

        } catch (SQLException e) {
            e.printStackTrace();
            System.out.println(e.getMessage());

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public List<Movie> GetMoviesOfDirector(int did) {

        List<Movie> qResult = new LinkedList<Movie>();

        try {
            Statement statement = _connection.createStatement();

            ResultSet rs = statement.executeQuery(String.format(
                    "SELECT m.mid, m.title, m.genre, m.year, m.did, m.rating, d.name " +
                    "FROM movie m, director d " +
                    "WHERE m.did=%d AND d.did=m.did;", did));

            while (rs.next()) {
                 qResult.add(new Movie(rs.getInt("mid"), rs.getString("title"),
                    rs.getString("genre"), rs.getFloat("rating"), rs.getInt("year"),
                    new Director(rs.getInt("did"), rs.getString("name"))));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }

        return qResult;
    }

    @Override
    public List<Movie> GetMoviesOfActor(int aid) {
    
        List<Movie> qResult = new LinkedList<Movie>();

        try {
            Statement statement = _connection.createStatement();

            ResultSet rs = statement.executeQuery(String.format(
                    "SELECT m.mid, m.title, m.genre, m.year, m.did, m.rating, d.name " +
                    "FROM movie m, casted_in c, director d " +
                    "WHERE c.aid=%d AND c.mid=m.mid AND d.did=m.did;", aid));

            while (rs.next()) {
                 qResult.add(new Movie(rs.getInt("mid"), rs.getString("title"),
                        rs.getString("genre"), rs.getFloat("rating"), rs.getInt("year"),
                        new Director(rs.getInt("did"), rs.getString("name"))));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }

        return qResult;
    }

    @Override
    public List<Movie> GetUserWatchlist(int uid) {
        
        List<Movie> qResult = new LinkedList<Movie>();
        
        try {
            Statement statement = _connection.createStatement();

            ResultSet rs = statement.executeQuery(String.format(
                    "SELECT m.mid, m.title, m.genre, m.year, m.did, m.rating, d.name " +
                    "FROM watched w, movie m, director d " +
                    "WHERE w.mid=m.mid AND w.uid=%d AND d.did=m.did;", uid));

            while (rs.next()) {
                 qResult.add(new Movie(rs.getInt("mid"), rs.getString("title"),
                         rs.getString("genre"), rs.getFloat("rating"), rs.getInt("year"),
                         new Director(rs.getInt("did"), rs.getString("name"))));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }

        return qResult;
    }

    @Override
    public List<Movie> GetMoviesWithTwoActors(String actorName1, String actorName2) {
        
        List<Movie> qResult = new LinkedList<Movie>();
        
        try {
            Statement statement = _connection.createStatement();
                    
            ResultSet rs = statement.executeQuery(String.format(
                "SELECT m.mid, m.title, m.genre, m.year, m.did, m.rating, d.name " +
                "FROM movie m, director d WHERE m.did=d.did "                      +
                	"AND m.mid IN ( "                                              +
               		 "SELECT c1.mid FROM casted_in c1, actor a1 "                  +
              		 "WHERE a1.name=\"%s\" AND c1.aid=a1.aid "                     +
                    ") AND m.mid IN ( "                                            +
		             "SELECT c2.mid FROM casted_in c2, actor a2 "                  +
 		             "WHERE a2.name=\"%s\" AND c2.aid=a2.aid ); "
            , actorName1, actorName2));

            while (rs.next()) {
                 qResult.add(new Movie(rs.getInt("mid"), rs.getString("title"),
                        rs.getString("genre"), rs.getFloat("rating"), rs.getInt("year"),
                        new Director(rs.getInt("did"), rs.getString("name"))));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }

        return qResult;
    }

    @Override
    public List<Movie> GetMoviesAboveRating(float rating) {

        List<Movie> qResult = new LinkedList<Movie>();
        
        try {
            Statement statement = _connection.createStatement();
                    
            ResultSet rs = statement.executeQuery(String.format(
                    "SELECT m.mid, m.title, m.genre, m.year, m.did, m.rating, d.name " +
                    "FROM movie m, director d WHERE m.rating > %.1f " +
                    "AND m.did=d.did;", rating));

            while (rs.next()) {
                 qResult.add(new Movie(rs.getInt("mid"), rs.getString("title"),
                        rs.getString("genre"), rs.getFloat("rating"), rs.getInt("year"),
                        new Director(rs.getInt("did"), rs.getString("name"))));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }

        return qResult;
    }

    @Override
    public void ChangeRatingsOfMoviesLike(String pattern, float value) {

        try {
            Statement statement = _connection.createStatement();
            
            statement.executeUpdate(String.format(
                "UPDATE movie SET rating=%.1f WHERE LOWER(title) LIKE \"%%%s%%\";",
                value, pattern.toLowerCase()));
        
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void DeleteDirectorWithId(int did) {
        
        try {
            Statement statement = _connection.createStatement();
                    
            statement.executeUpdate(
                    String.format("DELETE FROM director WHERE did=%d;", did));
        
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void DeleteUserWithId(int uid) {
        
        try {
            Statement statement = _connection.createStatement();
                    
            statement.executeUpdate(String.format("DELETE FROM user WHERE uid=%d;", uid));
        
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void PrintViewStatsByMovie() {
        
        try {
            Statement statement = _connection.createStatement();
                    
            ResultSet rs = statement.executeQuery(
                    "SELECT m.mid, m.title, COUNT(*) as viewCount " +
                    "FROM movie m, watched w " +
                    "WHERE m.mid=w.mid " +
                    "GROUP BY m.mid " +
                    "ORDER BY COUNT(*) DESC, m.title ASC;");

            while (rs.next()) {
                 System.out.println(String.format("%d|%s|%d", rs.getInt("mid"),
                         rs.getString("title"), rs.getInt("viewCount")));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void PrintViewStatsByGenre() {
     
        try {
            Statement statement = _connection.createStatement();
                    
            ResultSet rs = statement.executeQuery(
                    "SELECT m.genre, COUNT(*) as viewCount " +
                    "FROM movie m, watched w " +
                    "WHERE m.mid=w.mid " +
                    "GROUP BY m.genre " +
                    "ORDER BY COUNT(*) DESC, m.genre ASC;");

            while (rs.next()) {
                 System.out.println(String.format("%s|%d",
                         rs.getString("genre"), rs.getInt("viewCount")));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void PrintAverageMovieRatingOfDirectors() {
      
        try {
            Statement statement = _connection.createStatement();
                    
            ResultSet rs = statement.executeQuery(
                    "SELECT d.name, AVG(m.rating) as avgRating, COUNT(*) as viewCount " +
                    "FROM director d, movie m, watched w " +
                    "WHERE m.did=d.did AND w.mid=m.mid " +
                    "GROUP BY d.name " + //TODO d.did!!
                    "ORDER BY ROUND(AVG(m.rating), 1) DESC, d.name ASC;");

            while (rs.next()) {
                 System.out.println(String.format("%s|%.1f|%d", rs.getString("name"),
                        rs.getFloat("avgRating"), rs.getInt("viewCount")));
            }

        } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
        }
    }

    @Override
    public void DropTables() {

        try {
            Statement statement = _connection.createStatement();
                    
            statement.executeUpdate("DROP TABLE casted_in;");
            statement.executeUpdate("DROP TABLE watched;");
            statement.executeUpdate("DROP TABLE movie;");
            statement.executeUpdate("DROP TABLE director;");
            statement.executeUpdate("DROP TABLE actor;");
            statement.executeUpdate("DROP TABLE user;");

         } catch (Exception e) {
            e.printStackTrace();
            System.out.println(String.format("Error occured: %s", e.getMessage()));
         }
    }


    /* 
     - This main function is for helping you
     - You don't have to send your main function when you submit your code.
        
     - Use this template to continue coding. 
     - Just implement a function above, and call it with example parameters
     in main function below.
     */
    public static void main(String[] args) throws Exception {

        // Test script will be placed here

    }
}
