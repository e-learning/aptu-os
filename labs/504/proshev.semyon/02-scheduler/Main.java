import java.util.List;
import java.util.ArrayList;
import java.util.Scanner;
import java.io.File;
import java.io.FileNotFoundException;

public class Main {

	public static void main(String[] args) throws FileNotFoundException {
		Scanner sc = new Scanner(System.in);
		// Scanner sc = new Scanner(new File("input.txt"));

		int quantum = Integer.parseInt(sc.nextLine());
		List<Task> tasks = new ArrayList<>();

		while (sc.hasNextLine()) {
			tasks.add(
				TaskParser.parse(sc.nextLine())
			);
		}

		new Scheduler(quantum).calculate(tasks);
	}
}