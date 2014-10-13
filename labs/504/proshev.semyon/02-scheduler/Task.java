import java.util.Collections;
import java.util.List;

public class Task {

	public final String id;
	public final int availabilityTime;
	public final int duration;
	public final List<IOTask> ioTasks;

	public Task(String id, int availabilityTime, int duration, List<IOTask> ioTasks) {
		this.id = id;
		this.availabilityTime = availabilityTime;
		this.duration = duration;
		this.ioTasks = Collections.unmodifiableList(ioTasks);
	}

	public static class IOTask {

		public final int startTime;
		public final int duration;

		public IOTask(int startTime, int duration) {
			this.startTime = startTime;
			this.duration = duration;
		}
	}
}