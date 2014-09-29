import java.util.List;
import java.util.ArrayList;

public class TaskParser {

	private TaskParser() {
	}

	public static Task parse(String str) {
		String[] description = str.split(" ");
		
		String id = description[0];
		int availabilityTime = Integer.parseInt(description[1]);
		int duration = Integer.parseInt(description[2]);
		
		List<Task.IOTask> ioTasks = new ArrayList<>();

		for (int i = 0; i < description.length - 3; i += 2) {
			ioTasks.add(
                    new Task.IOTask(
                            Integer.parseInt(description[i + 3]),
                            Integer.parseInt(description[i + 4])
                    )
            );
		}

		return new Task(id, availabilityTime, duration, ioTasks);
	}
}