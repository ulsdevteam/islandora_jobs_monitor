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
          redFrom: 85, redTo: 100,
          yellowFrom: 61, yellowTo: 84,
          greenColor: '#99bf72',
          greenFrom: 1, greenTo: 60,
          minorTicks: 5
        };

        var chart = new google.visualization.Gauge(document.getElementById('<?php print $gauge_server_id; ?>'));

        chart.draw(data, options);
      }
    </script>