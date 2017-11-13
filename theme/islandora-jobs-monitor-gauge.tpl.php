<div id="<?php print $gauge_server_id; ?>"></div>
   <script type="text/javascript">
      google.charts.load('current', {'packages':['gauge']});
      google.charts.setOnLoadCallback(drawChart);

      function drawChart() {

        var data = google.visualization.arrayToDataTable([
          ['Label', 'Value'],
          ['Disk', <?php print $disk_used; ?>],
          ['CPU', <?php print $cpu; ?>],
          ['Memory', <?php print $memory; ?>]
        ]);

        var options = {
          width: 220, height: 88,
          redFrom: 90, redTo: 100,
          yellowFrom: 80, yellowTo: 90,
          greenColor: '#c9bfa2',
          greenFrom: 65, greenTo: 80,
          minorTicks: 5
        };

        var chart = new google.visualization.Gauge(document.getElementById('<?php print $gauge_server_id; ?>'));

        chart.draw(data, options);
      }
    </script>
